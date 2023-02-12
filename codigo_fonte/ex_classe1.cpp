#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

class subnivel {
private:
   char   simb[6];
   double Eb, rs;
   double coef[6];
public:
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
      printf("Simb: %s\n",simb);
      printf("Z: %d\tA: %lf\trho: %e\n",Z,A,rho);
      for ( i=0; i<6; i++ ) printf(" %e ",coer[i]);   printf("\n");
      for ( i=0; i<6; i++ ) printf(" %e ",incoer[i]); printf("\n");
      for ( j=0; j<ns; j++ ) {
         printf("Simb: %s\tEb: %lf\trs: %lf\n",
                                 sn[j].Simb(), sn[j].Ene(), sn[j].Rs());
         for ( i=0; i<6; i++ )
            printf(" %e ",sn[j].Coef(i));
         printf("\n");
      }
   }
};

int main(int argc, char *argv[]) {

   dbase    db;
 
   if (argc!=2) {
      printf("Parametros errados!!!\n");
      printf("\tUsar:\n");
      printf("\t./p <argumento>\n");
      exit(0);
   }

   db.Ler_Dados(argv[1]);
   db.Imprimir();
 
   return 0;
}

