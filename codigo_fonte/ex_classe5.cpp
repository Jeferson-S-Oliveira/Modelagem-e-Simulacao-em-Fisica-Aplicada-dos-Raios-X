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
   void Imprimir(void) {
      printf("( %lf, %lf, %lf )\n",x, y, z);
   }
   XYZ operator +(XYZ v) { // soma vetorial
      return XYZ( x+v.x, y+v.y, z+v.z );
   }
   XYZ operator -(XYZ v) { // subtração vetorial
      return XYZ( x-v.x, y-v.y, z-v.z );
   }
   XYZ operator -(void) { // negativo
      return XYZ( -x, -y, -z );
   }
   double operator *(XYZ v) { // produto escalar
      return x*v.x + y*v.y + z*v.z;
   }
   XYZ operator *(double d) { // escala
      return XYZ(x*d, y*d, z*d);
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
      XYZ r(x,y,z), v, res;
      double s;
      // usando quaternions
      s = cos(0.5*ang);
      v = n.Unitario()*sin(0.5*ang);
      res = (r*(s*s)) - (r*(v*v)) + (v*(2*(v*r))) + ((v*(2*s))^r);
      return res; 
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

class foton {
private:
   double hv;   // energia
   XYZ p, d;    // posicao e direcao

public:
   foton(void) { hv=0; }
   foton(double e, XYZ ip, XYZ id) {
      hv = e;
      p = ip;
      d = id;
   }
   //foton(foton f) { *this = f; } ia servir para copiar um foton para o outro, mas não está funcionando no momento
   // atribuir valores
   void HV(double e) { hv = e; } // atribuir energia ao foton
   void POS(XYZ ip) { p = ip; } // atribuir posicao ao foton
   void DIR(XYZ id) { d = id; } // atribuir direcao ao foton
   // retornar valores
   double HV(void)  { return hv; } // retonra energia ao foton
   XYZ    POS(void) { return p; } // retonra posicao ao foton
   XYZ    DIR(void) { return d; } // retonra direcao ao foton
   // translacao
   void Translacao (XYZ dist) { p = p + dist; }
   // rotacao
   void Rotacao(double ang, XYZ n) { d = d.Rotacao(ang,n); }
   void Imprimir(void) {
      printf("hv = %lf\n", hv);
      printf("p = "); p.Imprimir();
      printf("d = "); d.Imprimir();
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
   dbase(void){}
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
   }
};

int main(int argc, char *argv[]) {

   dbase    db;
   int      i;
   double   E0 = 50; // keV
   double   x, x0 = 1.0; // cm
   Info     prob;
   double   eps;
   
   foton f(1.0,XYZ(1,0,0),XYZ(0,0,1));
   f.Imprimir();
   f.Translacao(XYZ(2,1,1));
   f.Imprimir();
   f.Rotacao(M_PI,XYZ(0,1,0));
   f.Imprimir();

   XYZ p(1,0,0), d(0,1,0), q;

   printf("p: "); p.Imprimir();
   printf("d: "); d.Imprimir();

   q = p*5;
   printf("escala p*5: ");
   q.Imprimir();

   q = -p;
   printf("negativo de p: ");
   q.Imprimir();

   eps = p*d;
   printf("produto escalar p*d: %lf\n",eps);

   q = p^d;
   printf("produto vetorial p^d: ");
   q.Imprimir();

   q = p.Rotacao(0.5*M_PI,d);
   printf("rotacao de p M_PI/2 em torno de d: ");
   q.Imprimir();

   q = XYZ(0,0,1).Rotacao(2*M_PI/3,XYZ(1,1,1));
   printf("rotacao de (1,0,0) 2M_PI/3 em torno de (1,1,1): ");
   q.Imprimir();


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






