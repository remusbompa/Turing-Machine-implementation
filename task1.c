#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct
{
	char *crt_state,*next_state;
	char tape1_read_symbol,tape2_read_symbol,
		tape1_write_symbol,tape2_write_symbol,
		tape1_direction,tape2_direction;
}Tranzitie;

typedef struct 
{
	int N;
	char **K;
	int M;
	char **F;
	char *s0;
	int P;
	Tranzitie **d;
}MT;

void Eroare()
{
	printf("Eroare alocare memorie!");
	exit(0);
}

char* FindState(FILE *fin,MT *m)
{
	char *w=(char*)calloc(100,sizeof(char));
	if(!w) Eroare();
	fscanf(fin,"%s",w);
	int i;
	for(i=0;i<m->N;i++)
		if(!strcmp(m->K[i],w))
			{free(w);
			return m->K[i];}
	free(w);
	return NULL;
}

void CitireTranz(FILE *fin,MT *m,int nr)
{
	m->d[nr]->crt_state=FindState(fin,m);
	fscanf(fin," %c",&m->d[nr]->tape1_read_symbol);
	fscanf(fin," %c",&m->d[nr]->tape2_read_symbol);
	m->d[nr]->next_state=FindState(fin,m);
	fscanf(fin," %c",&m->d[nr]->tape1_write_symbol);
	fscanf(fin," %c",&m->d[nr]->tape1_direction);
        fscanf(fin," %c",&m->d[nr]->tape2_write_symbol);
        fscanf(fin," %c",&m->d[nr]->tape2_direction);
}

void CitireMT(FILE *fin,MT *m,char *tape1,char *tape2)
{
	int i;
	fscanf(fin,"%d",&m->N);
	m->K=(char**)calloc(m->N,sizeof(char*));
	if(!m->K) Eroare();
	for(i=0;i<m->N;i++)
		{m->K[i]=(char*)calloc(100,sizeof(char));
		if(!m->K[i]) Eroare();
		fscanf(fin,"%s",m->K[i]);}
	fscanf(fin,"%d",&m->M);
	m->F=(char**)calloc(m->M,sizeof(char*));
	if(!m->F) Eroare();
	for(i=0;i<m->M;i++)
		m->F[i]=FindState(fin,m);
	m->s0=FindState(fin,m);
	fscanf(fin,"%d",&m->P);
	m->d=(Tranzitie**)calloc(m->P,sizeof(Tranzitie*));
	if(!m->d) Eroare();
	for(i=0;i<m->P;i++)
		{m->d[i]=(Tranzitie*)malloc(sizeof(Tranzitie));
		if(!m->d[i]) Eroare();
		CitireTranz(fin,m,i);}
	fscanf(fin,"%s",tape1);
	fscanf(fin,"%s",tape2);
}

void ElibMT(MT *m)
{
	int i;
	for(i=0;i<m->N;i++)
		free(m->K[i]);
	free(m->K);
	free(m->F);
	for(i=0;i<m->P;i++)
		free(m->d[i]);
	free(m->d);
	free(m);
}

void AfiMT(MT *m,char *tape1,char *tape2)
{
	int i;
	printf("%d\n",m->N);
	for(i=0;i<m->N;i++)
		printf("%s ",m->K[i]);
	printf("\n%d\n",m->M);
	for(i=0;i<m->M;i++)
		printf("%s ",m->F[i]);
	printf("\n%s\n%d\n",m->s0,m->P);
	for(i=0;i<m->P;i++)
		printf("%s %c %c %s %c %c %c %c\n",m->d[i]->crt_state,m->d[i]->tape1_read_symbol,
		m->d[i]->tape2_read_symbol,m->d[i]->next_state,m->d[i]->tape1_write_symbol,
		m->d[i]->tape1_direction,m->d[i]->tape2_write_symbol,m->d[i]->tape2_direction);
	printf("%s\n%s\n",tape1,tape2);
}

void InitTape(char *tape,int n)
{
        int i;
        for(i=n;i<10000;i++)
                tape[i]='#';
}

void MarginiTape(char *tape,int *ms,int *md)
{
	*ms=1;
	*md=10000-1;
	while(tape[*ms]=='#')
		(*ms)++;
	while(tape[*md]=='#')
		(*md)--;
}
void AfiTape(char *tape,FILE* fout)
{
        int i;
	int ms,md;
	MarginiTape(tape,&ms,&md);
        for(i=ms;i<=md;i++)
        	fprintf(fout,"%c",tape[i]);
	fprintf(fout,"\n");
}

int StareFinala(MT *m,char *crt_state)	
{
	int i;
	for(i=0;i<m->M;i++)
		if(m->F[i]==crt_state) return 1;
	return 0;
}

Tranzitie* AflaTranzitia(MT*m,char *tape1,char *tape2,int c1,int c2,char* st_in)
{
	int i;
	for(i=0;i<m->P;i++)
		{Tranzitie* t=m->d[i];
		if(st_in==t->crt_state&&tape1[c1]==t->tape1_read_symbol&&tape2[c2]==t->tape2_read_symbol)
			return t;}
	return NULL;
}

void MutCursor(int *c,char direction)
{
	if(direction=='L')
		(*c)--;
	if(direction=='R')
		(*c)++;
}

void RezolvareMT(MT *m,char *tape1,char *tape2,FILE *fout)
{
	InitTape(tape1,strlen(tape1));
	InitTape(tape2,strlen(tape2));
	int c1=1,c2=1;
	char* st_in=m->s0;
	Tranzitie *t=AflaTranzitia(m,tape1,tape2,c1,c2,st_in);
	while(t)
		{printf("%s->%s\n",st_in,t->next_state);
		tape1[c1]=t->tape1_write_symbol;
		tape2[c2]=t->tape2_write_symbol;
		MutCursor(&c1,t->tape1_direction);
		MutCursor(&c2,t->tape2_direction);
		st_in=t->next_state;
		t=AflaTranzitia(m,tape1,tape2,c1,c2,st_in);}
	if(StareFinala(m,st_in)) 
		{AfiTape(tape1,fout);
		AfiTape(tape2,fout);}
	else fprintf(fout,"The machine has blocked!\n");		
}

int main()
{
	FILE *fin=fopen("task1.in","r");
	MT *m=(MT*)malloc(sizeof(MT));
	if(!m) Eroare();
	char tape1[10000],tape2[10000];
	CitireMT(fin,m,tape1,tape2);
	fclose(fin);
	AfiMT(m,tape1,tape2);
	FILE *fout=fopen("task1.out","w");
	RezolvareMT(m,tape1,tape2,fout);
	fclose(fout);
	ElibMT(m);
	return 0;	
}
