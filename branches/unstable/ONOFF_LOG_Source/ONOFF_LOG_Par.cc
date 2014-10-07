// Calculates ON-OFF Source parameters from discrete traffic statistics,
// which this source will try to simulate

#include <omnetpp.h>
#include <math.h>
#include <fstream>
#include <string>
#include <vector>

class ONOFF_LOG_Par : public cSimpleModule
{
  protected:
    simtime_t timeslot;      // time between packets
    int cellsize;            // cell size in bytes
    
    cXMLElement *doc;        // XML file with traffic statistics

    long int ns;             // number of individual ON-OFF sources 

    // Fields and function to calculate ACF shape parameters
    double H;
    std::vector<double>  nacf;
    std::vector<double>  shapePars;
    double R(double tau, double x, double y);
    double differ(double x, double y);
    double MS, MS2, Pon; 
    double amoeba2D(double F[3],double X[3], double Y[3],
                    double ftol,int *nfunk );
    double amotry2d(double F[3], double X[3], double Y[3],
                    double c);
    
    // Function to calculate ON-OFF individual source parameters
    std::vector<double>  dst;
    cStdDev ispeed;
    double sleepLambda;
    double  getSleepParameter(double Eoff);
    void Dconv(double* v1, int size1, double* v2, int size2, double* r);
    double* ONOFFsrcProb(int N);
    
    // The following redefined virtual function holds the algorithm.
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

// The module class needs to be registered with OMNeT++
Define_Module(ONOFF_LOG_Par);


#define gDEBP(A,B) (A->getDocumentElementByPath(A,B))->getNodeValue()


void ONOFF_LOG_Par::initialize()
{
    // Initialize is called at the beginning of the simulation.

    EV<<"Source parameters calculation started"<<endl;

    // traffic data input
    doc=par("data");
    timeslot = atof(gDEBP(doc,"/TIMESLOT"));
    cellsize = atoi(gDEBP(doc,"/CELLSIZE"));     
    
    H        = atof(gDEBP(doc,"//HURST"));

    cStringTokenizer tok1(gDEBP(doc,"//ACF-VALUES"));
    nacf = tok1.asDoubleVector();
  
    cStringTokenizer tok2(gDEBP(doc,"/TRAFFIC-DISTRIBUTION"));
    dst  = tok2.asDoubleVector();

    double EY,S;
    EY = atof(gDEBP(doc,"//CELLSPERSLOT"));
   
    // ONOFF parameters calculations and output
    std::ofstream out( par("profile").stringValue() );
    out<<"<?xml version=\'1.0' ?>"<<endl<<endl;
    out<<"<ONOFF_PARAMETERS>"<<endl;
    out<<" <TIMESLOT> " << timeslot << " </TIMESLOT>"<<endl;
    out<<" <CELLSIZE> " << cellsize << " </CELLSIZE>"<<endl;

    // ON-OFF individual sources parameters output  
    out<<" <INDIVIDUAL-SOURCES>"<<endl;
    ns= getParentModule()->par("numSources").longValue();
    out<<"  <SOURCE-NUMBER> "<< ns << " </SOURCE-NUMBER>"<<endl;
     
    Pon = 1-pow(dst[0], 1.0/ns);
    S= EY/ns/Pon;

    out<<"  <MAXIMAL-SPEED> "<< S <<" </MAXIMAL-SPEED>"<<endl;
    out<<"  <SOURCE-SPEED> "<< S <<"  </SOURCE-SPEED>"<<endl;
   
    // Normalized ACF shape parameters calculation
    MS  = S; 
    MS2 = MS*MS;
    

    double X[3]={0.5, 0.5, 0.1}, Y[3]={0.1, 0.5, 0.5};
    double f[3], tol, alpha=4-2*H;
    int count=0;
    f[0]= differ(X[0], Y[0]);
    f[1]= differ(X[1], Y[1]);
    f[2]= differ(X[2], Y[2]);
    tol = amoeba2D(f, X, Y, 1.e-8, &count );
    double px=X[0], py=Y[0];

    int i;
    double s, xx;    

    for(s=py,i=2; (xx=pow(px+i,-alpha))/s > 1.0e-9; s+=xx,i++);
    int imax=i; 
    double A=(1-py)/(s-py);

    for(s=py,i=2; i<=imax; i++) s+=i*A*pow(px+i,-alpha);
    out<<"  <ACTIVETIME-MEAN> "<< s << " </ACTIVETIME-MEAN>"<<endl;

    out<<"  <SLEEP-PROBABILITY> "<<  1-Pon <<" </SLEEP-PROBABILITY>"<<endl;
    out<<"  <SLEEPTIME-MEAN> "<< (1-Pon)*s/Pon <<" </SLEEPTIME-MEAN> "<<endl;
    sleepLambda=getSleepParameter((1-Pon)*s/Pon);
    out<<"  <SLEEP-PARAMETERS> "<< sleepLambda <<" </SLEEP-PARAMETERS> " <<endl;

    out<<" </INDIVIDUAL-SOURCES>"<<endl;

    out<<" <ACF-SHAPE-PARAMETERS>  ";
    // alpha - tail power, px - tail shift, py - prob of 1
    out<< alpha <<"  "<< px <<"  "<< py <<"  "<< A <<"  "<< imax <<endl;
    out<<"  </ACF-SHAPE-PARAMETERS>";
    
    out<<"</ONOFF_PARAMETERS>"<<endl;
    out.close();

    EV<<"Source parameters calculation complete"<<endl;      
}

void ONOFF_LOG_Par::handleMessage(cMessage *msg)
{
    // The handleMessage() method is called whenever a message arrives
    // at the module.
    delete msg;
}


/* --------------------------------------------------------------- */
#define TINY 1.0e-10  /* A small number. */
#define NMAX 5000
#define SWAP(i,j) {(swap)=F[i];F[i]=F[j];F[j]=(swap);\
(swap)=X[i];X[i]=X[j];X[j]=(swap);(swap)=Y[i];Y[i]=Y[j];Y[j]=(swap);}

double ONOFF_LOG_Par::amotry2d(double F[3], double X[3], double Y[3],
                double c)
{
   double Ftry, Xtry, Ytry;

   Xtry=(1-c)*0.5*(X[0]+X[1]) + c*X[2];
   Ytry=(1-c)*0.5*(Y[0]+Y[1]) + c*Y[2];
   Ftry=differ(Xtry,Ytry);
   if( Ftry<F[2] ) {F[2]=Ftry; X[2]=Xtry; Y[2]=Ytry;}
   return Ftry;
}


double ONOFF_LOG_Par::amoeba2D(double F[3],double X[3], double Y[3],
                               double ftol,int *nfunk )
{
double swap, rtol, Ftry, Fsave;

*nfunk=0;
while(1) {

   /* Sorting to find best (F[0]), next to worst (F[1]) and worst (F[2])
      simplex point */
   if( F[0]>F[1] ) SWAP(0,1)
   if( F[1]>F[2] ) SWAP(1,2)
   if( F[0]>F[1] ) SWAP(0,1)
   /* Compute the fractional range from highest to lowest and return
      if satisfactory (return also if too many iterations made). */
   rtol=2.0*fabs(F[2]-F[0])/(fabs(F[2])+fabs(F[0])+TINY);
   if (rtol < ftol || *nfunk >= NMAX) return rtol;

   /* Begin a new iteration. First extrapolate by a factor -1 through
      the face of the simplex across from the high point, i.e., reflect
      the simplex from the high point. */
   Ftry = amotry2d(F,X,Y, -1);  (*nfunk)++;
   /* Gives a result better than the best point, so try an additional
      extrapolation by a factor 2. */
   if (Ftry <= F[0]) { Ftry = amotry2d(F,X,Y, 2);  (*nfunk)++; }
   else if (Ftry >= F[1]) {
      /* The reflected point is worse than the second-highest, so look for
         an intermediate lower point, i.e., do a one-dimensional contraction. */
      Fsave= F[2];
      Ftry = amotry2d(F,X,Y, 0.5);  (*nfunk)++;
      if (Ftry >= Fsave) {
         /* Can’t seem to get rid of that high point.
            Better contract around the lowest (best) point. */
         F[1]=differ( X[1]=0.5*(X[0]+X[1]), Y[1]=0.5*(Y[0]+Y[1]) );
         F[2]=differ( X[2]=0.5*(X[0]+X[2]), Y[2]=0.5*(Y[0]+Y[2]) );
         *nfunk+=2;
         }
      }
   }  /* end of while(1) */
}

void ONOFF_LOG_Par::Dconv(double* v1, int size1, 
                                        double* v2, int size2, 
                                        double* r )
{
    int i, j, k;
    for(k=0; k<size1+size2-1; k++) 
       for(r[k]=0, i=0, j=k; i<size1 && j>=0; i++, j--)
          if( j<size2 ) r[k]+=v1[i]*v2[j]; 
}


double* ONOFF_LOG_Par::ONOFFsrcProb(int N)
{
    // Speed of individual source for N ON-OFF sources traffic model calculation

    int i,m,k;
    double h,sum,p_off;
    double* srcPDF = new double[dst.size()];
    double* bu     = new double[N+1];
    double* conv[dst.size()];
    int     csize[dst.size()];

    // Bernulli probabilities is kept in array to save calculation time
    bu[0]=dst[0];
    p_off=pow(bu[0], 1.0/N);
    for(i=1; i<=N; i++) bu[i]=bu[i-1]*(1-p_off)/p_off*(N-i+1)/i;

    // individual source speed PDF calculation begins
    // first two probabilites 
    srcPDF[0]=0;    // no fictive sources condition
    srcPDF[1]= sum = dst[1]/bu[1];

    // other probabilities
    // to save time in convolution calculation, we keep all convolution results
    conv[0] = new double[dst.size()];
    for(i=2; i<(int)dst.size() && sum<1; i++) {
       csize[0]=i;
       for(k=0; k<i; k++) { conv[0][k]=srcPDF[k];}
       for(m=2, h=0; m<=i && m<=N && h<=dst[i]; m++) {
          if(conv[m-1] != NULL) { delete[] conv[m-1]; conv[m-1]=NULL; }
          conv[m-1] = new double[ csize[m-1]=csize[m-2]+i-1 ];
          Dconv(conv[m-2], csize[m-2], srcPDF, i, conv[m-1]); 
          h += bu[m]*conv[m-1][i];  
          }
       if( dst[i] > h ) srcPDF[i]=((dst[i]-h)/bu[1]);
       else srcPDF[i]=0;  
       sum+=srcPDF[i];
       }

    // correction to make sum of all probabilities be equal to one
    srcPDF[--i]+=1-sum;
    for (++i; i<(int)dst.size(); i++) srcPDF[i]=0;
    
    delete[] bu;
    return srcPDF;
}


double ONOFF_LOG_Par::getSleepParameter(double Eoff)
{
    if(Eoff<0) { 
       EV<<"getSleepParameter:incorrect arguments!"<<endl;
       return 0;
       }

    return Eoff;
}


static double PrA(int n, double alpha, double A, double x, double y)
{
if( n==1) return y;
if( n>1 ) return A*pow(x+n, -alpha);
else return 0;
}


static double PrB(int n, double lambda)
{
double h=exp(-lambda);
if( n>=0 ) {
   for(int i=1; i<=n; i++) h*=lambda/i;   
   }
else return 0;
return h;
}


static double PrAplusB(int n, double alpha, double A, double x, double y, double lambda)
{
double h=0;
for(int i=0; i<=n; i++) h+=PrA(i,alpha,A,x,y)*PrB(n-i,lambda);
return h;
}

#define IMAX 100

static double g(int n, double alpha, double A, double x, double y, double lambda)
{
static int im=-1;
static double gg[IMAX];
int i,k;
double h;

if(n<=im) return gg[n];
else {
   if(n==0) { gg[0]=0; im=0; return 0; }
   if(n==1) { gg[1]=1; im=1; return 1; }
   for(k=im+1; k<=n; k++) {
      h=1;
      for(i=1; i<k; i++) h-=PrA(i,alpha,A,x,y);
      for(i=1; i<k; i++) h+=gg[k-i]*PrAplusB(i,alpha,A,x,y,lambda);
      gg[k]=h;
      }
   im=n;
   return gg[n];
   }
}


double ONOFF_LOG_Par::R(double tau, double x, double y)
{
int i, imax, k, n, m;
double MA, r1, r2, r3, h, hh;

double alpha=4-2*H;
for(h=y,i=2; (hh=pow(x+i,-alpha))/h > 1.0e-6; h+=hh,i++);
imax=i; 
double A=(1-y)/(h-y);
for(MA=y, i=2; i<=imax; i++) MA+=i*A*pow(x+i,-alpha);
double lambda=(1-Pon)*MA/Pon;

h=0;
for(k=0; k<tau; k++) {
   hh=1;
   for(n=1; n<=k; n++) hh-=PrA(n,alpha,A,x,y);
   h+=hh;
   }
r1=MS2*Pon*(1-h/MA);

h=0;
for(k=0; k<tau; k++)
   for(n=0; n<tau-k; n++) {
      hh=1;
      for(m=1; m<=k; m++) hh-=PrA(m,alpha,A,x,y);
      h+=hh*PrB(n,lambda)*g(tau-k-n,alpha,A,x,y,lambda);
      }
r2= MS*MS*Pon*h/MA;

r3=-MS*MS*Pon*Pon;

return r1+r2+r3;
}

#define FIT_POINTS 5

double ONOFF_LOG_Par::differ(double x, double y)
{
int k;
double h, nr[FIT_POINTS+1], R0;

R0=R(0,x,y);
for(h=0, k=1; k<=FIT_POINTS; k++) {
   nr[k]=R(k,x,y)/R0;
   h+=(nacf[k]-nr[k])*(nacf[k]-nr[k]);
   }  
return h;
}

