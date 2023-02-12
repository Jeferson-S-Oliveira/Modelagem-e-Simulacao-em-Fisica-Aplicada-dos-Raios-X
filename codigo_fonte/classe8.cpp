#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

#define graus (M_PI/180.0)

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
      printf("( %+lf, %+lf, %+lf )\n",x, y, z);
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

//         printf("res1.ok: %d\tres1.q: ",res1.ok); res1.q.Imprimir();
//         printf("res2.ok: %d\tres2.q: ",res2.ok); res2.q.Imprimir();
//         printf("\n");

         if (!res1.ok && res2.ok) {
            res1 = res2;
         } else if (res1.ok && res2.ok) {
            if ((pos-res2.q).Norma()<(pos-res1.q).Norma()) {
               res1 = res2;
            }
         }
      }
//      printf("res1.ok: %d\tres1.q: ",res1.ok); res1.q.Imprimir();
//      printf("\n");
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
   // impressao
   void Imprimir(void) {
      printf("hv = %lf\n",hv);
      printf("p = "); p.Imprimir();
      printf("d = "); d.Imprimir();
      printf("\n");
   }
};

class fonte {
private:
   int     nc;   // numero de canais
   double *spe;  // espectro de probabilidade acumulada
   double *en;   // canais de energia
   double  theta; // angulo de abertura
   XYZ     p, d; // posicao e direcao
public:
   fonte(void) { nc = 0; theta = 0; spe = NULL; en = NULL; }
   // atribuir valores
   void Pos(XYZ ip)    { p = ip;   } // atribuir posicao a fonte
   void Dir(XYZ id)    { d = id;   } // atribuir direcao a fonte
   void Theta(double a) { theta = a; } // atribuir angulo de abertura (radianos)

   void Ler_Dados(const char *nome_spe) {
      FILE *fp;
      char  lixo[32];
      int   i;

      fp = fopen(nome_spe,"rt");
      if (!fp) {
         printf("Erro ao abrir arquivo de espectro %s!!!\n",nome_spe);
         exit(0);
      }

      for ( nc = 0; !feof(fp)?fgets(lixo,32,fp):0; nc++ );

      printf("nc: %d\n",nc);

      spe = new double[nc];
      en  = new double[nc];

      fseek(fp,0,SEEK_SET);
      for ( i=0; i<nc; i++ ) {
         fscanf(fp,"%lf %lf ", &en[i], &spe[i]);
      }

      fclose(fp);

      for ( i=1; i<nc; i++ ) spe[i] += spe[i-1];
      for ( i=0; i<nc; i++ ) spe[i] /= spe[nc-1];

   }

   foton Emitir(void) {
      int    i;
      double eps;
      XYZ    r, s;
      foton  hv;

      if (!nc) {
         printf("Espectro não definido!!!\n");
         exit(0);
      }

      // sortear energia do foton
      eps = rand()/(double)RAND_MAX;
      for ( i=0; i<nc?eps>spe[i]:0; i++ );
      hv.Hv(en[i]);

      // atribuir posicao do foton
      hv.Pos(p); // posicao da fonte

      // determinar direcao do foton
      // rotacao em torno de eixo perpendicular a direcao de emissao
      eps = rand()/(double)RAND_MAX;
      r = d.Rotacao( atan(tan(theta)*eps), d^XYZ(0,0,1) );

      // rotacao em torno da direcao de emissao
      eps = rand()/(double)RAND_MAX;
      s = r.Rotacao( 2*M_PI*eps, d); // isoprovavel

      // atribuir direcao do foton
      hv.Dir(s);

      return hv;
   }

   void Imprimir(void) {
      printf("=====================================================\n");
      printf("Posicao : "); p.Imprimir();
      printf("Direcao : "); d.Imprimir();
      printf("Abertura: %lf graus\n", theta/graus);
      printf("=====================================================\n");
      printf("\tEnergia\t\tProbabilidade\n");
      for ( int i=0; i<nc; i++ ) {
         printf("\t%lf\t%lf\n", en[i], spe[i]);
      }
      printf("=====================================================\n");
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

   void AbsFotoeletrica(foton *hv) {
      hv->Hv(0); // zera a energia; absorvido
   }

   void EspalhamentoElastico(foton *hv) {
      double eps, theta;
      double Pth;
      XYZ d, r, s;
      // SCD Thomson
      // dsigma/dtheta = re^2*(1+cos^2(theta))/2
      // P(theta) = 0.5*(1+cos^2(theta))
      do {
         theta = 2*M_PI*rand()/(double)RAND_MAX;
         Pth = 0.5*(1+cos(theta)*cos(theta));
         eps = rand()/(double)RAND_MAX;
      } while (eps>Pth);
      // atualiza direcao
      d = hv->Dir();
      r = d.Rotacao(theta,(d^XYZ(0,0,1)).Unitario());
      eps = rand()/(double)RAND_MAX;
      hv->Dir(r.Rotacao(2*M_PI*eps,d));
   }

   void EspalhamentoInelastico(foton *hv) {
      double eps, theta;
      double Ec_E, k;
      double Pcp;
      XYZ d, r, s;
      // SCD Compton
      // k = E/(me*c^2)=E/511;
      // Ec = E/(1+k*(1-cos(theta)))
      // dsigma/dtheta = re^2*(Ec/E)(Ec/E + E/Ec - sin^2(theta))/2
      // P(theta) = 0.5*(Ec/E)*(Ec/E + E/Ec - sin^2(theta))
      k = hv->Hv()/511;
      do {
         theta = 2*M_PI*rand()/(double)RAND_MAX;
         Ec_E = 1/(1+k*(1-cos(theta)));
         Pcp = 0.5*(Ec_E)*(Ec_E + 1/Ec_E - sin(theta)*sin(theta));
         eps = rand()/(double)RAND_MAX;
      } while (eps>Pcp);
      // atualiza energia espalhamento
      hv->Hv(hv->Hv()*Ec_E);
      // atualiza direcao
      d = hv->Dir();
      r = d.Rotacao(theta,(d^XYZ(0,0,1)).Unitario());
      eps = rand()/(double)RAND_MAX;
      hv->Dir(r.Rotacao(2*M_PI*eps,d));
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

   int      i;
   dbase    db;
   solido   sol;
   fonte    f;
   foton    hv;
   InfoGeo  pt;
   Info     prob;
   double   nev, x, eps;

   if (argc!=5) {
      printf("Parametros errados!!!\n");
      printf("\tUsar:\n");
      printf("\t./p <eventos> <atomo>.MU <modelo>.off <espectro>.txt\n");
      exit(0);
   }

   srand(time(NULL)); 

   nev = atof(argv[1]);

   db.Ler_Dados(argv[2]);
//   db.Imprimir();

   sol.Ler_Dados(argv[3]);
   sol.Imprimir();

   f.Ler_Dados(argv[4]);
   f.Pos(XYZ(-5,0,0));
   f.Dir(XYZ(1,0,0));
   f.Theta(5*graus);
   f.Imprimir();

   printf("\n\n");
   for ( i=0; i<nev; i++ ) {
      printf("----------------------------------------------------------------\n");
      hv = f.Emitir(); hv.Imprimir();

      pt = sol.Intersecao(hv.Pos(),hv.Dir());
      printf("\tok: %d",pt.ok);
      if (pt.ok) {
         printf(" (entrou)\tq: "); pt.q.Imprimir();
         // colocar o foton sobre o modelo
         hv.Pos(pt.q);
         // loop de interacoes
         while (hv.Hv()>0.5) {
            // determina ponto de saida
            pt = sol.Intersecao(hv.Pos(),hv.Dir());
            printf("\tok: %d",pt.ok);
            if (pt.ok) {
               printf(" \t\tq: "); pt.q.Imprimir();
               // calcula passo e probabilidades
               prob = db.Informacao(hv.Hv());
               x = (pt.q-hv.Pos()).Norma();
               if (prob.s>x) {
                  printf("\t\tsem interacao\n");
                  // colocar o foton sobre em pt.q
                  hv.Pos(pt.q);
                  hv.Imprimir();
               } else {
                  // atualiza posicao do foton
                  hv.Pos(hv.Pos()+hv.Dir()*prob.s);
                  hv.Imprimir();
                  // sorteia mecanismo de interacao
                  eps = rand()/(double)RAND_MAX;
                  if (eps<prob.pabs) {
                     printf("\t\tabsorcao fotoeletrica\n");
                     db.AbsFotoeletrica(&hv);
                  } else if (eps<prob.pabs+prob.pee) {
                     printf("\t\tespalhamento elastico\n");
                     db.EspalhamentoElastico(&hv);
                  } else {
                     printf("\t\tespalhamento inelastico\n");
                     db.EspalhamentoInelastico(&hv);
                  }
               }
            } else {
               printf(" (saiu)\n");
               break;
            }
         }
         if (hv.Hv()>0.5) {
            // testar se chegou no detector
            //
         }
      } else {
         // sem colisao com o modelo
         // foton perdido
         printf(" (sem colisao)\n");
      }
      printf("----------------------------------------------------------------\n");
   }
   return 0;
}



