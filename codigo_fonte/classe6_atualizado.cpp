#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

class XYZ {
protected:
   double x, y, z;

public:
   XYZ(void) { x = y = z = 0; } // construtor XYZ()
   XYZ(double ix, double iy, double iz) { // construtor XYZ(x,y,z)
      x = ix; y = iy; z = iz;
   }
   XYZ operator +(XYZ v) { // soma vetorial
      return XYZ( x+v.x, y+v.y, z+v.z );
   }
   XYZ operator -(XYZ v) { // subtracao vetorial
      return XYZ( x-v.x, y-v.y, z-v.z );
   }
   XYZ operator -(void) { // negativo
      return XYZ( -x, -y, -z );
   }
   double operator *(XYZ v) { // produto escalar
      return x*v.x + y*v.y + z*v.z;
   }
   XYZ operator *(double d) { // escala: XYZ*double
      return XYZ(x*d, y*d, z*d);
   }
   friend XYZ operator *(double d,XYZ v) { // escala: double*XYZ
      return v*d;
   }
   XYZ operator /(double d) { // escala
      return XYZ(x/d, y/d, z/d);
   }
   XYZ operator ^(XYZ v) { // produto vetorial
      return XYZ(y*v.z-v.y*z, z*v.x-v.z*x, x*v.y-v.x*y);
   }
   double Norma(void) {
      return sqrt((*this)*(*this));
   }
   XYZ Unitario(void) {
      double d = Norma();
      return (*this)/d;
   }
   XYZ Rotacao(double ang, XYZ n) {
      XYZ v, res;
      double s;
      // usando quaternions
      s = cos(0.5*ang);
      v = n.Unitario()*sin(0.5*ang);

      // versao usando *this
      //res = ((*this)*(s*s)) - ((*this)*(v*v)) + (v*(2*(v*(*this)))) + ((v*(2*s))^(*this));

      XYZ r(x,y,z);
      res = (r*(s*s)) - (r*(v*v)) + (v*(2*(v*r))) + ((v*(2*s))^r);

      return res; 
   }
   void Imprimir(void) {
      printf("( %lf, %lf, %lf )\n",x, y, z);
   }
};
typedef struct { int ok; XYZ q; } InfoGeo;
class face {
private:
   int   nv; // total de vertices da face
   XYZ   *v; // lista de vertices
   XYZ    n; // normal ao plano da face
   double d; // distancia do plano da face

public:
   face(void) { nv = 0; v = NULL; }
   face(int tv, XYZ *lv) {
      nv = tv; v = lv;
      n = ((v[2]-v[1])^(v[0]-v[1])).Unitario();
      d = -n*v[0];
   }
   InfoGeo Intersecao(XYZ pos, XYZ dir){
      InfoGeo res;
      double s = -(n*dir); // se vai ao encontro da face
      double t = (n*pos)+d; //  distancia a face
      
      res.ok = 0; // considerar ponto fora
      if ((s>0 && t>0) || (s<0 && t<0)) {
         res.q = pos+dir*(t/s); // ponto de intersecao no plano da face
         res.ok = 1;   // partindo do principio que o ponto esta dentro
         for ( int i=0; i<nv; i++ ) {
            res.ok = res.ok && // testando se o ponto esta a esquerda
               (((v[(i+1)%nv]-v[i])^(res.q-v[i]))*n)>0;  // da aresta
         }
      }
      return res;
   }
   void Imprimir(void) {
      printf("nvf: %d\n",nv);
      for ( int i=0; i<nv; i++ ) {
         printf("\tv[%d]: ",i); v[i].Imprimir();
      }
      printf("\tn(%lf): ",d); n.Imprimir();
   }
};

class solido {
private:
   int nf;  // total de faces
   face *f; // lista de faces

public:
   solido(void) { nf = 0; f = NULL; }
   void Ler_Dados(char *nome_model) {
      FILE  *fp;
      int    nv, id, nvf;
      double x, y, z;
      XYZ   *v; // lista temporaria de vertice
      XYZ   *vf; // lista de vertices de cada face

      fp = fopen(nome_model,"rt");
      if (!fp) {
         printf("Erro ao abrir o arquivo %s!!!\n",nome_model);
         exit(0);
      }
      fscanf(fp,"OFF ");
      fscanf(fp,"%d %d 0 ",&nv,&nf);
      v = new XYZ[nv];
      for ( int i=0; i<nv; i++ ) {
         fscanf(fp,"%lf %lf %lf ", &x, &y, &z);
         v[i] = XYZ(x,y,z);
      }
      f = new face[nf]; // aloca memoria para lista de faces
      for ( int i=0; i<nf; i++ ) { // leitura das faces
         fscanf(fp,"%d ", &nvf);
         vf = new XYZ[nvf]; // aloca memoria para lista de vertices da face
         for ( int j=0; j<nvf; j++ ) { // leitura dos indices 
            fscanf(fp,"%d ", &id);
            vf[j] = v[id];    // lista de vertices
         }
         f[i] = face(nvf,vf); // construcao da face
      }
      delete [] v; // libera memoria da lista de vertices
   }
   InfoGeo Intersecao(XYZ pos, XYZ dir) {
      InfoGeo res1, res2;
      res1 = f[0].Intersecao(pos,dir);
      for ( int i=1; i<nf; i++ ) {
         res2 = f[i].Intersecao(pos,dir);

         printf("res1.ok: %d\tres1.q: ",res1.ok); res1.q.Imprimir();
         printf("res2.ok: %d\tres2.q: ",res2.ok); res2.q.Imprimir();
         printf("\n");

         if (!res1.ok && res2.ok) {
            res1 = res2;
         } else if (res1.ok && res2.ok) {
            if ((pos-res2.q).Norma()<(pos-res1.q).Norma()) {
               res1 = res2;
            }
         }
      }
      printf("res1.ok: %d\tres1.q: ",res1.ok); res1.q.Imprimir();
      printf("\n");
      
      return res1;
   }
   void Imprimir(void) {
      for (int i=0; i<nf; i++ )
         f[i].Imprimir();
   }
};

class foton {
private:
   double hv;    // energia
   XYZ    p, d;  // posicao e direcao

public:
   foton(void)                     { hv=0;                   }
   foton(double e, XYZ ip, XYZ id) { hv = e; p = ip; d = id; }
   // atribuir valores
   void Hv(double e) { hv = e; } // atribuir energia ao foton
   void Pos(XYZ ip)  { p = ip; } // atribuir posicao ao foton
   void Dir(XYZ id)  { d = id; } // atribuir direcao ao foton
   // retornar valores
   double Hv(void)  { return hv; } // retorna energia do foton
   XYZ    Pos(void) { return p;  } // retorna posicao do foton
   XYZ    Dir(void) { return d;  } // retorna direcao do foton
   // translacao
   void Translacao(XYZ dist) { p = p + dist; }
   // rotacao
   void Rotacao(double ang, XYZ n) { d = d.Rotacao(ang,n); }
   void Imprimir(void) {
      printf("hv = %lf\n",hv);
      printf("p = "); p.Imprimir();
      printf("d = "); d.Imprimir();
      printf("\n");
   }
};

typedef struct {
   double pabs, pee, pei, s;
} Info;

class subnivel {
private:
   char   simb[6];
   double Eb, rs;
   double coef[6];
public:
   subnivel(void) {}
   // interface para atribuicao de dados
   void Simb(char *nome)            { strcpy(simb,nome); }
   void Ene(double val)             { Eb = val; }
   void Rs(double val)              { rs = val; }
   void Coef(int idx, double val)   { coef[idx] = val; }
   // interface para leitura de dados
   char  *Simb(void)    { return simb; }
   double Ene(void)     { return Eb; }
   double Rs(void)      { return rs; }
   double Coef(int idx) { return coef[idx]; }

   double SC_AbsFoto(double e) {
      int    i;
      double lnE = log(e);
      double sec = 0;
      double sum = 0;
      if (e>Eb) {
         for ( i=0; i<6; i++ ) {
            sum += coef[i]*pow(lnE,i);
         }
         sec = exp(sum);
      }
      return sec;
   }
};

class dbase {
private:
   char     simb[3];
   int      Z;
   double   A, rho;
   double   coer[6], incoer[6];
   int      ns;
   subnivel *sn;

public:
   dbase(void) {}
   double Den(void) { return rho; }

   double SC_AbsFoto(double e) {
      int i;
      double sum = 0;
      for ( i=0; i<ns; i++ ) {
         sum += sn[i].SC_AbsFoto(e);
      }
      return sum;
   }
   double SC_Elastico(double e) {
      int    i;
      double lnE = log(e);
      double sum = 0;
      for ( i=0; i<6; i++ ) {
         sum += coer[i]*pow(lnE,i);
      }
      return exp(sum);
   }
   double SC_Inelastico(double e) {
      int    i;
      double lnE = log(e);
      double sum = 0;
      for ( i=0; i<6; i++ ) {
         sum += incoer[i]*pow(lnE,i);
      }
      return exp(sum);
   }

   Info Informacao(double e) {
      Info info;
      double tau = SC_AbsFoto(e);
      double sigma_e = SC_Elastico(e);
      double sigma_i = SC_Inelastico(e);
      double mu = tau + sigma_e + sigma_i;
      double eps = rand()/(double)RAND_MAX;

      info.pabs = tau/mu;
      info.pee = sigma_e/mu;
      info.pei = sigma_i/mu;
      info.s = -log(eps)/(mu*rho);

      return info;
   }

   void Ler_Dados(char *nome_atomo) {
      FILE  *fp;
      char   sb[6],lixo[128];
      int    pos;
      int    i, j;
      double Eb, coef, rs;

      fp = fopen(nome_atomo,"rt");
      if (fp==NULL) {
         printf("Problema ao acessar o arquivo!!!\n");
         exit(0);
      }
      fscanf(fp,"%s",simb);

      fgets( lixo, 127, fp );
      //printf("lixo: %s\n",lixo);

      // Ler numero atomico, massa atomica e densidade
      fscanf(fp," %d %lf %lf ",&Z,&A,&rho);

      // Pular linha de dados desnecessarios
      fgets( lixo, 127, fp );
      //printf("lixo: %s\n",lixo);

      // Ler coeficientes do espalhamento elastico
      for ( i=0; i<6; i++ ) fscanf(fp," %lf ",&coer[i]);
      // Ler coeficientes do espalhamento inelastico
      for ( i=0; i<6; i++ ) fscanf(fp," %lf ",&incoer[i]);

      // Pular linha de dados desnecessarios
      fgets( lixo, 127, fp );
      //printf("lixo: %s\n",lixo);

      pos = ftell(fp);
//      printf("pos: %d\n",pos);

      ns = 0;
      for (;;) {
         ns++;
         fgets( lixo, 127, fp );
         if (lixo[0]=='g') break;
      }
      ns /= 3;
//      printf("ns: %d\n",ns);

      // sn = (subnivel *) malloc(ns*sizeof(subnivel));
      sn = (subnivel*) new subnivel[ns];

      fseek(fp,pos,SEEK_SET);
      for ( j=0; j<ns; j++ ) {
         fscanf(fp," %s %lf %lf ",sb,&Eb,&rs);
         sn[j].Simb(sb);
         sn[j].Ene(Eb);
         sn[j].Rs(rs);
         for ( i=0; i<6; i++ ) {
            fscanf(fp," %lf ",&coef);
            sn[j].Coef(i,coef);
         }

      }

      fclose(fp);
   }

   void Imprimir(void) {
      int i, j;
      printf("=============================================================\n");
      printf("Simb: %s\n",simb);
      printf("=============================================================\n");
      printf("Z: %d\tA: %lf\trho: %e\n",Z,A,rho);
      for ( i=0; i<6; i++ ) printf(" %+7.3e ",coer[i]);   printf("\n");
      for ( i=0; i<6; i++ ) printf(" %+7.3e ",incoer[i]); printf("\n");
      for ( j=0; j<ns; j++ ) {
         printf("-------------------------------------------------------------\n");
         printf("Simb: %s\tEb: %lf\trs: %lf\n",
                            sn[j].Simb(), sn[j].Ene(), sn[j].Rs());
         for ( i=0; i<6; i++ )
            printf(" %+7.3e ",sn[j].Coef(i));
         printf("\n");
      }
      printf("=============================================================\n");
      printf("\n");
   }
};

int main(int argc, char *argv[]) {

   dbase    db;
   int      i;
   double   E0 = 50; // keV
   double   x, x0 = 1.0; // cm
   Info     prob;
   double   eps;
   InfoGeo  pt;

   solido sol;
   sol.Ler_Dados(argv[1]);
   sol.Imprimir();

   pt = sol.Intersecao(XYZ(-5,0,0),XYZ(1,0,0));
   printf("ok: %d\tq: ",pt.ok); pt.q.Imprimir();
   
   foton f(E0,XYZ(1,0,0),XYZ(0,0,1));
   f.Imprimir();
   f.Translacao(XYZ(2,1,1));
   f.Imprimir();
   f.Rotacao(M_PI,XYZ(0,1,0));
   f.Imprimir();


//   XYZ p(1,0,0), d(0,1,0), q;

//   printf("p: "); p.Imprimir();
//   printf("d: "); d.Imprimir();

//   q = p*5;
//   printf("escala p*5: ");
//   q.Imprimir();

//   q = -p;
//   printf("negativo de p: ");
//   q.Imprimir();

//   eps = p*d;
//   printf("produto escalar p*d: %lf\n",eps);

//   q = p^d;
//   printf("produto vetorial p^d: ");
//   q.Imprimir();

//   q = p.Rotacao(0.5*M_PI,d);
//   printf("rotacao de p M_PI/2 em torno de d: ");
//   q.Imprimir();

//   q = XYZ(0,0,1).Rotacao(2*M_PI/3,XYZ(1,1,1));
//   printf("rotacao de (1,0,0) 2M_PI/3 em torno de (1,1,1): ");
//   q.Imprimir();


//   srand(time(NULL)); 

//   if (argc!=2) {
//      printf("Parametros errados!!!\n");
//      printf("\tUsar:\n");
//      printf("\t./p <argumento>\n");
//      exit(0);
//   }

//   db.Ler_Dados(argv[1]);
////   db.Imprimir();

//   for ( i=0; i<100; i++ ) {
//      prob = db.Informacao(E0);
//      if (prob.s>x0) {
//         printf("%lf\t%lf\t------> Atravessou\n",prob.s,x0);
//      } else {
//         eps = rand()/(double)RAND_MAX;
//         if (eps<prob.pabs) {
//            printf("%lf\t%lf\tXXXXXXX Absorcao fotoeletrica\n",
//                                                      eps,prob.pabs);
//         } else if (eps<prob.pabs+prob.pee) {
//            printf("%lf\t%lf\tXXXXXXX Espalhamento elastico\n",
//                                                      eps,prob.pabs+prob.pee);
//         } else {
//            printf("%lf\t%lf\tXXXXXXX Espalhamento inelastico\n",eps,1.0);
//         }
//      }
//   }
   return 0;
}


