
#include "BinomCD.h"


#include <rational.h>
#include <val_utils.h>
#include <analysis.h>


wxFrame *MyFrame = nullptr;


#ifdef _WIN32
std::string valdir = val::CurrentHomeDir()+"\\AppData\\Roaming\\MVPrograms",
            settingsdir = valdir + "\\BinomCD", settingsfile=settingsdir + "\\settings.txt",
            iconpath="C:\\gccprogrammes\\BinomCD\\icons\\MV_Binom.xpm",
            alticonpath = val::GetExeDir() + "\\MV_Binom.xpm";
#else
//std::string iconpath="/home/miguel/gccprogrammes/BinomCD/icons/MV_Binom.xpm",filesep="/";
std::string settingsdir = val::CurrentHomeDir() + "/.config/MVPrograms/BinomCD",valdir= val::CurrentHomeDir() + "/.config/MVPrograms";
std::string settingsfile = settingsdir + "/settings.conf";
std::string iconpath = val::CurrentHomeDir() + "/.local/share/icons/MVPrograms/MV_Binom.xpm", alticonpath ="";
#endif // _WIN32

wxDEFINE_EVENT(MY_EVENT,MyThreadEvent);



/*
double round(const double& x,int k=4)
{
    double y=val::abs(x),faktor=1,d=0,z=1,y1;
    unsigned limit=~0;
    int i;

    //limit = ~limit;
    //std::cout<<" "<<limit<<"  ";

    for(i=0;i<k;++i) z*=10.0;

    y*=z;

    //std::cout<<" "<<y<<"  ";

    while (faktor<y) faktor*=10;
    if (faktor>y) faktor/=10;
    //std::cout<<" "<<faktor<<"  ";
    if (y<double(limit)) {
        //std::cout<<" "<<limit<<"  ";
        d=unsigned(y);
        y-=d;
        if (y>=0.5) ++d;
        if (x<0) return -d/z;
        else return d/z;
    }
    y1=y;
    while (faktor>=1) {
        d+= double(unsigned(y1/faktor)) *faktor;
        y1-=double(unsigned(y1/faktor)) *faktor;
        faktor/=10;
    }
    //std::cout<<" "<<d<<"  ";
    y-=d;
    if (y>=0.5) ++d;
    if (x<0) return -d/z;
    else return d/z;

}

*/

double Phi(double x)
{
    // constants
    double a1 =  0.254829592;
    double a2 = -0.284496736;
    double a3 =  1.421413741;
    double a4 = -1.453152027;
    double a5 =  1.061405429;
    double p  =  0.3275911;

    // Save the sign of x
    int sign = 1;
    if (x < 0)
        sign = -1;
    x = val::abs(x)/val::sqrt(2.0);

    // A&S formula 7.1.26
    double t = 1.0/(1.0 + p*x);
    double y = 1.0 - (((((a5*t + a4)*t) + a3)*t + a2)*t + a1)*t*val::exp(-x*x);

    return 0.5*(1.0 + sign*y);
}



double binom(int n, int k)
{
 int i,m;
 double b;

 if (k==0) return double(1);
 if (k==1) return double(n);
 if (n==k) return double(1);
 if (k==n-1) return double(n);

 m=n+1-k;
 b=double(n+1-k);
 for (i=2;i<=k;i++) {
     m++;
     b*=double(m);
     b/=(double(i));
 }
 return b;
}

double normalcd(const double& mu,const double& sigma,int k)
{
    return Phi((k+0.5-mu)/sigma);
}


double binomcd(int n,int k1,int k2,const double &p)
{
 int i;
 double c,b,q=1-p;

 if (k1>k2) return 0;
 if (n<k2) return 0;

 c=b=binom(n,k1) *val::power(p,k1)*val::power(q,n-k1);
 for (i=k1+1;i<=k2;i++){
     //cout<<endl<<b;
     c*= (p/q)*(double(n-i+1)/double(i));
     b+=c;
 }
 return b;
}


val::Glist<std::string> getwordsfromstring(const std::string &sf,const val::d_array<char>& separators,int emptywords,
                                           const val::d_array<char> &ignore)
{
    val::Glist<std::string> values;
    std::string s="";
    int n = sf.length();

    for (int i = 0; i < n ; ++i) {
        if (val::isinContainer(sf[i],ignore)) continue;
        if (val::isinContainer(sf[i],separators)) {
            if (emptywords || s != "") values.push_back(s);
            //else if (s != "") values.push_back(s);
            s = "";
        }
        else s += sf[i];
    }

    if (emptywords || s!= "") values.push_back(s);

    return values;
}


void computeCD(const double &p,int n,int k1,int k2,wxString &s_output)
{
    double mu,sigma,h;

    s_output="";
    wxString s_mu = L"\u03BC", s_sigma = L"\u03C3";

    mu = double(n)*p;
    sigma = val::sqrt(mu*(1.0-p));
    s_output=" " + s_mu + " = " + val::ToString(val::round(mu)) + " , " + s_sigma + " = " + val::ToString(val::round(sigma));
    h=binomcd(n,k1,k2,p);
    s_output+="\n binomcd  = "+ val::ToString(h) + "   " + val::ToString(val::round(h));
    if (k1 > k2) h= 0;
    else h = normalcd(mu,sigma,k2)-normalcd(mu,sigma,k1-1);
    s_output+="\n normalcd = "+ val::ToString(h) + "   " + val::ToString(val::round(h));

    MyThreadEvent event(MY_EVENT);
    if (MyFrame!=NULL) MyFrame->GetEventHandler()->QueueEvent(event.Clone() );
}


void hypothesentest(double p, int n, double alpha, test_type type)
{
    int k1=0, k2 = n;
    double accept = 1 - alpha, h;
    switch (type) {
        case test_type::L :
        {
            for (k1 = 0; k1 <= n; ++k1) {
                if ((h = binomcd(n, k1, k2, p)) < accept) {
                    --k1;
                    break;
                }
            }
        }
        break;
        case test_type::R :
        {
            for (k2 = n; k2 >= 0; --k2) {
                if ((h = binomcd(n, k1, k2, p)) < accept) {
                    ++k2;
                    break;
                }
            }
        }
        break;
        case test_type::B :
        {
            alpha /= 2.0; accept = 1-alpha;
            for (k1 = 0; k1 <= n; ++k1) {
                if ((h = binomcd(n, k1, n, p)) < accept) {
                    --k1;
                    break;
                }
            }
            for (k2 = n; k2 >= 0; --k2) {
                if ((h = binomcd(n, 0, k2, p)) < accept) {
                    ++k2;
                    break;
                }
            }
        }
        break;
        default: break;
    }
    //std::cout << n << "  " << k1 << " " << k2 << std::endl;
    MyThreadEvent event(MY_EVENT,TEST);
    event.SetNumbers(k1, k2);
    if (MyFrame!=NULL) MyFrame->GetEventHandler()->QueueEvent(event.Clone() );
}
