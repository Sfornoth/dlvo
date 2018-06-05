#ifndef MECHSYS_LBM_DOMAIN_H
#define MECHSYS_LBM_DOMAIN_H


#include <mechsys/dem/domain.h>
#include <mechsys/lbm/Lattice.h>
#include <mechsys/lbm/Interacton.h>
#include <mechsys/dem/domain.h>
#include <mechsys/lbm/Dem.h>

// STD
#include <map>
#include <vector>
#include <utility>
#include <set>
#include <ctime>
#include <ratio>
#include <chrono>

namespace LBM{

class Domain
{
public:
    static const double   WEIGHTSD2Q9   [ 9] ;
    static const Vec3_t   LVELOCD2Q9    [ 9] ;
    static const size_t   OPPOSITED2Q9  [ 9] ;
    static const double   MD2Q9     [ 9][ 9] ;
    // static const double   WEIGHTSD3Q15  [15] ;
    // static const Vec3_t   LVELOCD3Q15   [15] ;
    // static const size_t   OPPOSITED3Q15 [15] ;
    // static const double   MD3Q15    [15][15] ;
    static const double   WEIGHTSD3Q19  [19] ;
    static const Vec3_t   LVELOCD3Q19   [19] ;
    static const size_t   OPPOSITED3Q19 [19] ;
    // static const double   MD3Q19    [19][19] ;
    Domain();
    // ~Domain();
    void *UserData;
    typedef void (*ptDFun_t) (Domain &dom, void *UserData);

    double ****F;
    double ****Ftemp;
    double ****q;
    double ****Omeis;
    double ***Rho;
    double Rho0;
    Vec3_t ***Vel;
    Vec3_t ***BForce;
    double ***Check;
    bool ***IsSolid;
    double ***Gamma;
    double ****Sum;
    size_t Nneigh;
    double *EEk;
    double Sc;
    const double *W;
    const Vec3_t *C;
    const size_t *Op;
    size_t nx ;
    size_t ny ;
    size_t nz;
    iVec3_t Dim;
    size_t Nproc;
    size_t Ncells; // Remember might cause issue;
    double nu;
    double beta;
    double Tau;
    double dx;
    double dt;
    double Cs;
    double Alpha;
    double ratio; //Ll/L
    double ratiot; //tl/t
    double ratiom; //ml/m
    double ratiov;
    double Time;
    double Ttotal;
    double tout;
    bool IsF;
    bool Isq;
    int Type;
    Vec3_t ***VelP;
    // double ***VelP_Sum;
    Vec3_t ***Flbm;
    bool ***IsInside;
    bool ***IsBoundary;
    Vec3_t g;
    //MRT
    Mat_t M; 
    Mat_t Minv;
    Vec_t S;
    Vec_t meq;
    Array <LBM::Disk     *>                            Disks;         ///< Array of Disks for 2D calculation

	#ifdef USE_OMP
    omp_lock_t      lck;             ///< to protect variables in multithreading
	#endif
    //function
    void idx2Pt(size_t i, iVec3_t &iv);
    double Feq(size_t k, double rho, Vec3_t &vel);
    void WriteXDMF(char const * FileKey);
    void Collide();
    void CollideMRT();
    void Stream();
    void BounceBack();
    void BounceBackLIBB();
    void CalcForce();
    void CalcProps();
    void AddDisk(Vec3_t &pos, double R);
    void SetZeroGammaVelP();
    void AllocateDomainMem(size_t nx, size_t ny, size_t nz, double nu0, double dx0, double dt0);
    void Solve(double Tf, double dtOut, ptDFun_t ptSetup, ptDFun_t ptReport, char const* TheFileKey, bool RenderVideo);
};

#include "Output.h"

inline Domain::Domain ()
{
    Alpha = 1.0;
    Nproc = 1;
	omp_init_lock(&lck);
}
inline void Domain::AllocateDomainMem(size_t nx0, size_t ny0, size_t nz0,double nu0, double dx0, double dt0)
{
    IsF = false;
    Isq = false;
    nx = nx0;
    ny = ny0;
    nz = nz0;
    Dim = nx, ny, nz;
    Ncells = nx*ny*nz;
    dx = dx0;
    dt = dt0;
    Cs = dx/dt;
    Nneigh = 9;
	Ncells = nx*ny*nz;
	W      = WEIGHTSD2Q9;
    C      = LVELOCD2Q9;
    Op     = OPPOSITED2Q9;
	Sc = 0.17;
    Alpha = 1.0;
    beta = 0.0;
    Time = 0.0;
    Ttotal = 0.0;
    tout = 0.0;
	F = new double***[nx];
	Ftemp = new double***[nx];
    q = new double ***[nx];
	Omeis = new double***[nx];
    Rho = new double **[nx];
	Vel = new Vec3_t **[nx];
    VelP = new Vec3_t **[nx];
    Flbm = new Vec3_t **[nx];
    BForce = new Vec3_t **[nx];
    Check = new double **[nx];
	IsSolid = new bool **[nx];
    IsInside = new bool **[nx];
	IsBoundary = new bool **[nx];
    Gamma = new double **[nx];
	Sum = new double ***[nx];
    nu = nu0;
    Tau = 3.0*nu*dt/(dx*dx)+0.5;

	for (size_t i=0; i<nx; i++)
	{
		F[i] = new double **[ny];
		Ftemp[i] = new double **[ny];
        q[i] = new double **[ny];    
        Omeis[i] = new double **[ny];
		Rho[i] = new double *[ny];
		Vel[i] = new Vec3_t *[ny];
        VelP[i] = new Vec3_t *[ny];
        Flbm[i] = new Vec3_t *[ny];
        BForce[i] = new Vec3_t *[ny];
        Check[i] = new double *[ny];
		Gamma[i] = new double *[ny];
		Sum[i] = new double **[ny];
		IsSolid[i] = new bool *[ny];
        IsInside[i] = new bool *[ny];       
        IsBoundary[i] = new bool *[ny];		
		for (size_t j=0; j<ny; j++)
		{
			F[i][j] = new double*[nz];
			Ftemp[i][j] = new double*[nz];
            q[i][j] =  new double*[nz];
            Omeis[i][j] = new double*[nz];
			Rho[i][j] = new double [nz];
			Vel[i][j] = new Vec3_t [nz];
            VelP[i][j] = new Vec3_t [nz];
            Flbm[i][j] = new Vec3_t [nz];
            BForce[i][j] = new Vec3_t [nz];
			Check[i][j] = new double [nz];
			Gamma[i][j] = new double [nz];
            IsSolid[i][j] = new bool[nz];
            IsInside[i][j] = new bool[nz];
            IsBoundary[i][j] = new bool[nz];
            Sum[i][j] = new double *[nz];
			for (size_t k=0; k<nz; k++)
			{
				F[i][j][k] = new double[Nneigh];
				Ftemp[i][j][k] = new double[Nneigh];
                q[i][j][k] = new double[Nneigh];
                Omeis[i][j][k] = new double[Nneigh];
                Sum[i][j][k] = new double[3];
				IsSolid[i][j][k] = false;
                IsInside[i][j][k] = false;
                IsBoundary[i][j][k] = false;
                BForce[i][j][k] = 0.0, 0.0, 0.0;
                Flbm[i][j][k] = 0.0,0.0,0.0;
				for (size_t nn=0; nn<Nneigh; nn++)
				{
					F[i][j][k][nn] = 0.0;
					Ftemp[i][j][k][nn] = 0.0;
                    q[i][j][k][nn] = -1.0;
                    Omeis[i][j][k][nn] = 0.0;
				}
			}
		}
	}
	EEk = new double [Nneigh];
    for (size_t k=0;k<Nneigh;k++)
    {
        EEk[k]    = 0.0;
        for (size_t n=0;n<3;n++)
        for (size_t m=0;m<3;m++)
        {
            EEk[k] += fabs(C[k][n]*C[k][m]);
        }
    }

    M.Resize(Nneigh,Nneigh);
    Minv.Resize(Nneigh,Nneigh);
    for (size_t n=0; n<Nneigh; n++)
    {
        for (size_t m=0; m<Nneigh; m++)
        {
            M(n,m) = MD2Q9[n][m];
        }
    }
    Inv(M,Minv);
    double s = dx*dx/(3.0*dt*nu+0.5);
    s = 1.0/Tau;
    S.Resize(Nneigh);
    S = 1.0,1.4,1.4,1.0,1.2,1.0,1.2,s,s;
    // std::cout<<BForce[0][1][0]<<std::endl;
}



inline void Domain::AddDisk(Vec3_t &pos, double R)
{
    for(size_t iz=0; iz<nz; ++iz)
    for(size_t ix=0; ix<nx; ++ix)
    for(size_t iy=0; iy<ny; ++iy)
    {
        Vec3_t Cell(ix,iy,iz);
        double L = std::sqrt(dot(Cell - pos,Cell - pos));
        double Lx = Cell(0) - pos(0);
        double Ly = Cell(1) - pos(1);
        for(size_t k=0; k<Nneigh; ++k)
        {
            size_t nix = (size_t)((int)ix + (int)C[k](0) + (int)nx)%nx;
            size_t niy = (size_t)((int)iy + (int)C[k](1) + (int)ny)%ny;
            size_t niz = (size_t)((int)iz + (int)C[k](2) + (int)nz)%nz;
            Vec3_t Cell1(nix,niy,niz);
            double L1 = std::sqrt(dot(Cell1 - pos,Cell1 - pos));
            double l = std::sqrt(dot(Cell1 - Cell,Cell1 - Cell));
            double lx = Cell1(0) - Cell(0);
            double ly = Cell1(1) - Cell(1);
            double delta = 4.0*((lx*Lx+ly*Ly)*(lx*Lx+ly*Ly) - (l)*(l)*(L*L-R*R));
            if(delta<0)
            {
                q[ix][iy][iz][k] = -1;
                continue;
            }
            double q1 = (-(2.0*lx*Lx+2.0*ly*Ly) + std::sqrt(delta))/(2.0*l*l);
            double q2 = (-(2.0*lx*Lx+2.0*ly*Ly) - std::sqrt(delta))/(2.0*l*l);
            bool flag1 = q1>=0 && q1-1< 1e-5;
            // double flag1 = q1>=0;
            bool flag2 = q2>=0 && q2-1< 1e-5;
            bool flag3 = L>R;
            bool flag4 = L1<R;
            // double flag2 = q2>=0;
            if(flag3)
            {
                if(flag4)
                {
                    std::cout<<"!!! "<<q1<<" "<<q2<<std::endl;
                    if(flag1)
                    {
                        q[ix][iy][iz][k] = q1;
                        IsSolid[nix][niy][niz] = true;
                        if(flag2)
                        {
                            if(std::abs(q1-q2)>1e-6)
                            std::cout<<"!!! "<<q1<<" "<<q2<<std::endl;
                        }    
                    }else{
                        if(flag2)
                        {
                            q[ix][iy][iz][k] = q2;
                            IsSolid[nix][niy][niz] = true;
                                
                        }else{
                            q[ix][iy][iz][k] = -1;
                        }
                    }
                }
            }else{
            //    IsSolid[ix][iy][iz] = true; 
            }
        }
        // if(L<1.1*R) IsSolid[ix][iy][iz] = true;

    }
}

inline void Domain::idx2Pt(size_t i, iVec3_t &iv)
{
    iv(0) = i%Dim(0);
    iv(1) = (i/Dim(0))%Dim(1);
    iv(2) = i/(Dim(0)*Dim(1));
}

inline void Domain::CalcForce()
{
    #ifdef USE_OMP
    #pragma omp parallel for schedule(static) num_threads(Nproc)
    #endif
	// for(size_t ix=0; ix<nx;ix++)
	// for(size_t iy=0; iy<ny;iy++)
	// for(size_t iz=0; iz<nz;iz++)
    for(size_t i=0; i<Ncells; ++i)
	{
        iVec3_t iv(0,0,0);
        idx2Pt(i,iv);
        size_t ix = iv(0);
        size_t iy = iv(1);
        size_t iz = iv(2);
        Vec3_t Flbmt(0,0,0);
        for(size_t k=0; k<Nneigh; ++k)
        {
            double qt = q[ix][iy][iz][k];
            if(qt<0) continue;
            // size_t oix = (size_t)((int)ix + (int)C[Op[k]](0) + (int)nx)%nx;
            // size_t oiy = (size_t)((int)iy + (int)C[Op[k]](1) + (int)ny)%ny;
            // size_t oiz = (size_t)((int)iz + (int)C[Op[k]](2) + (int)nz)%nz;
            Flbmt += (Ftemp[ix][iy][iz][k]+F[ix][iy][iz][Op[k]])*C[k];
        }
        Flbm[ix][iy][iz] = Flbmt;
    }

}



inline double Domain::Feq(size_t k, double rho, Vec3_t &vel)
{
    double VdotV = dot(vel,vel);   
    double VdotC = dot(vel,C[k]);
    double Feq = W[k]*rho*(1.0 + 3.0*VdotC/Cs + 4.5*VdotC*VdotC/(Cs*Cs) - 1.5*VdotV/(Cs*Cs));
    return Feq;
}

inline void Domain::Collide()
{
    #ifdef USE_OMP
    #pragma omp parallel for schedule(static) num_threads(Nproc)
    #endif
	for(size_t ix=0; ix<nx;ix++)
	for(size_t iy=0; iy<ny;iy++)
	for(size_t iz=0; iz<nz;iz++)
	{
		if(!IsSolid[ix][iy][iz])
		{
    			double NonEq[Nneigh];
                double Q = 0.0;
                double tau = Tau;
                double rho = Rho[ix][iy][iz];
                Vec3_t vel = Vel[ix][iy][iz];
                double VdotV = dot(vel,vel);
                for (size_t k=0;k<Nneigh;k++)
                {
                    double VdotC = dot(vel,C[k]);
                    double FFeq   = W[k]*rho*(1.0 + 3.0*VdotC/Cs + 4.5*VdotC*VdotC/(Cs*Cs) - 1.5*VdotV/(Cs*Cs));
                    NonEq[k] = F[ix][iy][iz][k] - FFeq;
                    Q +=  NonEq[k]*NonEq[k]*EEk[k];
                }
                Q = sqrt(2.0*Q);
                tau = 0.5*(tau+sqrt(tau*tau + 6.0*Q*Sc/rho));
                tau = Tau;
                // double Bn = (Gamma[ix][iy][iz]*(tau-0.5))/((1.0-Gamma[ix][iy][iz])+(tau-0.5));
                // double S = 0;
                for (size_t k=0; k<Nneigh; k++)
                {
                    // std::cout<<ix<<" "<<iy<<" "<<iz<<" "<<k<<std::endl;
                    // std::cout<<BForce[0][1][0]<<std::endl;
                    double ForceTerm = dt*3.0*W[k]*dot(BForce[ix][iy][iz],C[k])/(Cs*Cs);                    
                	Ftemp[ix][iy][iz][k] = F[ix][iy][iz][k] -(NonEq[k]/tau)+ForceTerm;
                    // S += ForceTerm;
                    // if(Ftemp[ix][iy][iz][k]<0.0) Ftemp[ix][iy][iz][k] = 0.0;
                     
                }
                // std::cout<<S<<std::endl;
                
    	}else{
    		for(size_t k=0; k<Nneigh; k++)
    		{
    			Ftemp[ix][iy][iz][k] = F[ix][iy][iz][Op[k]];
    		}
    	}
    }
    // double **** tmp = F;
    // F = Ftemp;
    // Ftemp = tmp;
}

inline void Domain::CollideMRT()
{
    #ifdef USE_OMP
    #pragma omp parallel for schedule(static) num_threads(Nproc)
    #endif
	// for(size_t ix=0; ix<nx;ix++)
	// for(size_t iy=0; iy<ny;iy++)
	// for(size_t iz=0; iz<nz;iz++)
	// {
    for(size_t i=0; i<Ncells; ++i)
	{
        iVec3_t iv(0,0,0);
        idx2Pt(i,iv);
        size_t ix = iv(0);
        size_t iy = iv(1);
        size_t iz = iv(2);
		if(!IsSolid[ix][iy][iz])
		{
            double rho = Rho[ix][iy][iz];
            Vec3_t vel = Vel[ix][iy][iz];
            double f[Nneigh];
            double ft[Nneigh]; 
            double ft1[Nneigh];            
            for(size_t k=0; k<Nneigh; k++)
            {
                f[k] = F[ix][iy][iz][k];
                ft[k] = 0.0;
            }
            
            double fneq[Nneigh];
            memset(fneq,0,sizeof(fneq));
            int n = Nneigh, m=1;
            double a = 1.0, b = 0.0;
            dgemv_("N", &n, &n, &a, M.data, &n, f, &m, &b,ft,&m);
            // for(size_t i=0; i<Nneigh; i++)
            // {
            //     for(size_t j=0; j<Nneigh; j++)
            //     {
            //         ft[i] = 0.0;
            //         ft[i] += M(i,j)*f[j];                   
            //     }
            //     if(std::abs(ft[i]-ft1[i])<1e-6) std::cout<<ft[i]<<" "<<ft1[i]<<std::endl;
            // }
            double tau1 = 1/S(7);
            // double Bn = (Gamma[ix][iy][iz]*(tau1-0.5))/((1.0-Gamma[ix][iy][iz])+(tau1-0.5));
            // Bn = Gamma[ix][iy][iz];
            ft[0] = S(0)*(ft[0] - rho);
            ft[1] = S(1)*(ft[1] - rho*(-2.0 + 3.0*dot(vel,vel)));
            ft[2] = S(2)*(ft[2] - rho*( 1.0 - 3.0*dot(vel,vel)));
            ft[3] = S(3)*(ft[3] - rho*vel(0));
            ft[4] = S(4)*(ft[4] + rho*vel(0));
            ft[5] = S(5)*(ft[5] - rho*vel(1));
            ft[6] = S(6)*(ft[6] + rho*vel(1));
            ft[7] = S(7)*(ft[7] - rho*(vel(0)*vel(0)-vel(1)*vel(1)));
            ft[8] = S(8)*(ft[8] - rho*(vel(0)*vel(1)));
            //std::cout<<NonEq[5]<<" "<<F[ix][iy][iz][5]<<std::endl;
            dgemv_("N",&n,&n,&a,Minv.data,&n,ft,&m,&b,fneq,&m);
            // for(size_t i=0; i<Nneigh; ++i)
            // {
            //     for(size_t j=0; j<Nneigh; ++j)
            //     {
            //         fneq[i] = 0.0;
            //         fneq[i] += Minv(i,j)*ft[j];                   
            //     }
            // }
            for (size_t k=0; k<Nneigh; k++)
            {
                double ForceTerm = dt*3.0*W[k]*dot(BForce[ix][iy][iz],C[k])/(Cs*Cs); 
                Ftemp[ix][iy][iz][k] = F[ix][iy][iz][k] - fneq[k]+ForceTerm;
            	// Ftemp[ix][iy][iz][k] = F[ix][iy][iz][k] - ((1.0-Bn)*fneq[k] - Bn*Omeis[ix][iy][iz][k]);
            	// Ftemp[ix][iy][iz][k] = F[ix][iy][iz][k] - fneq[k];
                // double alpha = ((1.0-Bn)*fneq[k] - Bn*Omeis[ix][iy][iz][k])/F[ix][iy][iz][k];
                // if(alpha>1.0) Ftemp[ix][iy][iz][k] = 0.0;
                 
            }
		}else{
			for(size_t k=0; k<Nneigh; k++)
			{
				Ftemp[ix][iy][iz][k] = F[ix][iy][iz][Op[k]];
			}
		}
	}
    // double **** tmp = F;
    // F = Ftemp;
    // Ftemp = tmp;

}

inline void Domain::Stream()
{
    #ifdef USE_OMP
    #pragma omp parallel for schedule(static) num_threads(Nproc)
    #endif
	// for(size_t ix=0; ix<nx; ix++)
	// for(size_t iy=0; iy<ny; iy++)
	// for(size_t iz=0; iz<nz; iz++)
    for(size_t i=0; i<Ncells; ++i)
	{
        iVec3_t iv(0,0,0);
        idx2Pt(i,iv);
        size_t ix = iv(0);
        size_t iy = iv(1);
        size_t iz = iv(2);
        for(size_t k=0; k<Nneigh; k++)
        {
            size_t nix = (size_t)((int)ix + (int)C[k](0) + (int)nx)%nx;
            size_t niy = (size_t)((int)iy + (int)C[k](1) + (int)ny)%ny;
            size_t niz = (size_t)((int)iz + (int)C[k](2) + (int)nz)%nz;
            F[nix][niy][niz][k] = Ftemp[ix][iy][iz][k];
        }
    }
	
    // double **** tmp = F;
    // F = Ftemp;
    // Ftemp = tmp;
}

inline void Domain::BounceBack()
{
    #ifdef USE_OMP
    #pragma omp parallel for schedule(static) num_threads(Nproc)
    #endif
	// for(size_t ix=0; ix<nx; ix++)
	// for(size_t iy=0; iy<ny; iy++)
	// for(size_t iz=0; iz<nz; iz++)
    // {
    for(size_t i=0; i<Ncells; ++i)
	{
        iVec3_t iv(0,0,0);
        idx2Pt(i,iv);
        size_t ix = iv(0);
        size_t iy = iv(1);
        size_t iz = iv(2);
        if(IsBoundary[ix][iy][iz])
        {
            double Temp[Nneigh];
            for(size_t k=0; k<Nneigh; k++)
            {
                Temp[k] = Ftemp[ix][iy][iz][k];
            }
            // int flag[Nneigh];
            // memset(flag,0,sizeof(flag));
            for(size_t k=0; k<Nneigh; k++)
    	    {
                size_t nix = (size_t)((int)ix + (int)C[Op[k]](0) + (int)nx)%nx;
                size_t niy = (size_t)((int)iy + (int)C[Op[k]](1) + (int)ny)%ny;
                size_t niz = (size_t)((int)iz + (int)C[Op[k]](2) + (int)nz)%nz;
                
                if(IsSolid[nix][niy][niz])
                {   
    				F[ix][iy][iz][k] = Temp[Op[k]];
                    // flag[k] += 1;
                    // flag[Op[k]] += 1;
                    // std::cout<<"Direction "<<ix<<" "<<iy<<" "<<k<<std::endl;
                                                
                }
                
            }
            // double S = 0.0;
            // int index = -1;
            // for(size_t k=0; k<Nneigh;k++)
            // {
            //     // std::cout<<flag[k]<<",";
            //     if(std::abs(flag[k]-2)>1e-9)
            //     {
            //         // std::cout<<ix<<" "<<iy<<" "<<k<<std::endl;
            //         S += F[ix][iy][iz][k];                            
            //     }else{
            //         index = k;
            //     }
            // }
            // if(index>0)
            // {
            //     // std::cout<<ix<<" "<<iy<<" "<<std::endl;
                
            //     F[ix][iy][iz][index] = Ftemp[ix][iy][iz][index];
            //     F[ix][iy][iz][Op[index]] = Ftemp[ix][iy][iz][Op[index]];
            
            // }
            
                // std::cout<<std::endl;
        }
	    
    }
}

inline void Domain::BounceBackLIBB()
{
    #ifdef USE_OMP
    #pragma omp parallel for schedule(static) num_threads(Nproc)
    #endif
	// for(size_t ix=0; ix<nx; ix++)
	// for(size_t iy=0; iy<ny; iy++)
	// for(size_t iz=0; iz<nz; iz++)
    // {
    for(size_t i=0; i<Ncells; ++i)
	{
        iVec3_t iv(0,0,0);
        idx2Pt(i,iv);
        size_t ix = iv(0);
        size_t iy = iv(1);
        size_t iz = iv(2);
        double Temp[Nneigh];
        double Temp1[Nneigh];
        for(size_t k=0; k<Nneigh; k++)
        {
            Temp[k] = Ftemp[ix][iy][iz][k];
            size_t nix = (size_t)((int)ix + (int)C[k](0) + (int)nx)%nx;
            size_t niy = (size_t)((int)iy + (int)C[k](1) + (int)ny)%ny;
            size_t niz = (size_t)((int)iz + (int)C[k](2) + (int)nz)%nz;
            Temp1[k] = Ftemp[nix][niy][niz][k];
        }
        
        
        for(size_t k=0; k<Nneigh; k++)
        {
            double qt = q[ix][iy][iz][k];
            if(qt<0) continue;
            size_t oix = (size_t)((int)ix + (int)C[Op[k]](0) + (int)nx)%nx;
            size_t oiy = (size_t)((int)iy + (int)C[Op[k]](1) + (int)ny)%ny;
            size_t oiz = (size_t)((int)iz + (int)C[Op[k]](2) + (int)nz)%nz;
            size_t nix = (size_t)((int)ix + (int)C[k](0) + (int)nx)%nx;
            size_t niy = (size_t)((int)iy + (int)C[k](1) + (int)ny)%ny;
            size_t niz = (size_t)((int)iz + (int)C[k](2) + (int)nz)%nz;
            
            if(IsSolid[oix][oiy][oiz])
            {
                Ftemp[ix][iy][iz][k] = F[ix][iy][iz][k];
                if(qt<0.5)
                {
                    // if(IsSolid[nix][niy][niz]) qt = 0.5;
                    F[ix][iy][iz][k] = 2.0*qt*Temp[Op[k]]+(1-2.0*qt)*F[ix][iy][iz][Op[k]];                 
                }else{
                    F[ix][iy][iz][k] = (1.0/(2.0*qt))*Temp[Op[k]]+(1-1.0/(2.0*qt))*Temp[k];                                                             
                }

            }
            
            
        }
        

        

    }   
}
    
inline void Domain::CalcProps()    
{

    #ifdef USE_OMP
    #pragma omp parallel for schedule(static) num_threads(Nproc)
    #endif
	// for (size_t ix=0;ix<nx;ix++)
    // for (size_t iy=0;iy<ny;iy++)
    // for (size_t iz=0;iz<nz;iz++)
    // {
    for(size_t i=0; i<Ncells; ++i)
	{
        iVec3_t iv(0,0,0);
        idx2Pt(i,iv);
        size_t ix = iv(0);
        size_t iy = iv(1);
        size_t iz = iv(2);
        Vel   [ix][iy][iz] = Vec3_t(0.0,0.0,0.0);
        Rho   [ix][iy][iz] = 0.0;
        if(IsSolid[ix][iy][iz]) continue;
        for (size_t k=0;k<Nneigh;k++)
        {
            Rho[ix][iy][iz] +=  F[ix][iy][iz][k];
            Vel[ix][iy][iz] +=  F[ix][iy][iz][k]*C[k];
        }
        Vel[ix][iy][iz] *= Cs/Rho[ix][iy][iz];
    }
}

inline void Domain::SetZeroGammaVelP()
{
    #ifdef USE_OMP
    #pragma omp parallel for schedule(static) num_threads(Nproc)
    #endif
    // for(size_t ix=0; ix<nx; ix++)
    // for(size_t iy=0; iy<ny; iy++)
    // for(size_t iz=0; iz<nz; iz++)
    // {
    for(size_t i=0; i<Ncells; ++i)
	{
        iVec3_t iv(0,0,0);
        idx2Pt(i,iv);
        size_t ix = iv(0);
        size_t iy = iv(1);
        size_t iz = iv(2);
        Gamma[ix][iy][iz] = 0.0;
        // IsInside[ix][iy][iz] = false;
        // IsBoundary[ix][iy][iz] = false;
        Sum[ix][iy][iz][0] = 0.0;
        Sum[ix][iy][iz][1] = 0.0;        
        Sum[ix][iy][iz][2] = 0.0;        
        VelP[ix][iy][iz] = 0.0,0.0,0.0;
        Flbm[ix][iy][iz] = 0.0,0.0,0.0;
        for(size_t k=0; k<Nneigh; k++)
        {
            Omeis[ix][iy][iz][k] = 0.0;
        }
    }

}

void Domain::Solve(double Tf, double dtOut, ptDFun_t ptSetup, ptDFun_t ptReport, char const* TheFileKey, bool RenderVideo)
{
    String FileKey;
    FileKey.Printf("%s",TheFileKey);
    bool Finished = false;
    Util::Stopwatch stopwatch;
    printf("\n%s--- Solving ---------------------------------------------------------------------%s\n",TERM_CLR1   , TERM_RST);
    std::cout<<"Tau of fluid "<<Tau<<std::endl;
    IsF = false;
    

    
    size_t idx_out = 0;
    double tout = Time;
    while (Time < Tf)
    {
        if (Time >= tout)
        {
            if (TheFileKey!=NULL)
            {
                String fn;
				String fn1;
                fn.Printf    ("%s_%04d", TheFileKey, idx_out);
                
                if ( RenderVideo) 
                {
                    #ifdef USE_HDF5
                        WriteXDMF  (fn.CStr());
                    #endif
                }
            }
    
            tout += dtOut;
            idx_out++;
            printf(" Accomplished                                  = %3.2f %% \n", Time);
            if (ptReport!=NULL) (*ptReport) ((*this), UserData);            
        }
        SetZeroGammaVelP();
        CollideMRT();
        Stream();
        // BounceBack();
        BounceBackLIBB();
        CalcForce();
        if(ptSetup != NULL) (*ptSetup) ((*this), UserData); 
        
        CalcProps();
        Time += dt;
    }
    Finished = true;
    if (ptReport!=NULL) (*ptReport) ((*this), UserData);
    printf("%s  Final CPU time       = %s\n",TERM_CLR2, TERM_RST);
    
}



/*inline void Domain::LoadResults (char const * FileKey)
{
    String fn(FileKey);
    fn.append(".hdf5");
    if (!Util::FileExists(fn)) throw new Fatal("File <%s> not found",fn.CStr());
    printf("\n%s--- Loading file %s --------------------------------------------%s\n",TERM_CLR1,fn.CStr(),TERM_RST);
    hid_t file_id;
    file_id = H5Fopen(fn.CStr(), H5F_ACC_RDONLY, H5P_DEFAULT);
    int data[1];
    H5LTread_dataset_int(file_id,"NP",data);
    size_t NP = data[0];
}
*/
const double   Domain::WEIGHTSD2Q9   [ 9] = { 4./9., 1./9., 1./9., 1./9., 1./9., 1./36., 1./36., 1./36., 1./36.};
const Vec3_t   Domain::LVELOCD2Q9    [ 9] = { {0,0,0}, {1,0,0}, {0,1,0}, {-1,0,0}, {0,-1,0}, {1,1,0}, {-1,1,0}, {-1,-1,0}, {1,-1,0} };
const size_t   Domain::OPPOSITED2Q9  [ 9] = { 0, 3, 4, 1, 2, 7, 8, 5, 6 };
const double   Domain::MD2Q9 [ 9][ 9] =  { { 1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0},
{-4.0, -1.0, -1.0, -1.0, -1.0,  2.0,  2.0,  2.0,  2.0},
{ 4.0, -2.0, -2.0, -2.0, -2.0,  1.0,  1.0,  1.0,  1.0},
{ 0.0,  1.0,  0.0, -1.0,  0.0,  1.0, -1.0, -1.0,  1.0},
{ 0.0, -2.0,  0.0,  2.0,  0.0,  1.0, -1.0, -1.0,  1.0},
{ 0.0,  0.0,  1.0,  0.0, -1.0,  1.0,  1.0, -1.0, -1.0},
{ 0.0,  0.0, -2.0,  0.0,  2.0,  1.0,  1.0, -1.0, -1.0},
{ 0.0,  1.0, -1.0,  1.0, -1.0,  0.0,  0.0,  0.0,  0.0},
{ 0.0,  0.0,  0.0,  0.0,  0.0,  1.0, -1.0,  1.0, -1.0}};



// const double Domain::WEIGHTSD3Q15  [15] = { 2./9., 1./9., 1./9., 1./9., 1./9.,  1./9.,  1./9., 1./72., 1./72. , 1./72., 1./72., 1./72., 1./72., 1./72., 1./72.};
// const size_t Domain::OPPOSITED3Q15 [15] = { 0, 2, 1, 4, 3, 6, 5, 8, 7, 10, 9, 12, 11, 14, 13};                     ///< Opposite directions (D3Q15)
// const Vec3_t Domain::LVELOCD3Q15 [15] =
// {
// 	{ 0, 0, 0}, { 1, 0, 0}, {-1, 0, 0}, { 0, 1, 0}, { 0,-1, 0}, 
// 	{ 0, 0, 1}, { 0, 0,-1}, { 1, 1, 1}, {-1,-1,-1}, { 1, 1,-1}, 
// 	{-1,-1, 1}, { 1,-1, 1}, {-1, 1,-1}, { 1,-1,-1}, {-1, 1, 1} 
// };



const double Domain::WEIGHTSD3Q19  [19] = { 1./3., 1./18., 1./18., 1./18., 1./18., 1./18., 1./18., 1./36., 1./36., 1./36., 1./36., 1./36., 1./36., 1./36., 1./36., 1./36., 1./36., 1./36., 1./36.};
const size_t Domain::OPPOSITED3Q19 [19] = { 0, 2, 1, 4, 3, 6, 5, 8, 7, 10, 9, 12, 11, 14, 13, 16, 15, 18, 17};     ///< Opposite directions (D3Q19)
const Vec3_t Domain::LVELOCD3Q19 [19] =
{
	{ 0, 0, 0}, 
    { 1, 0, 0}, {-1, 0, 0}, { 0, 1, 0}, { 0,-1, 0}, { 0, 0, 1}, { 0, 0,-1}, 
    { 1, 1, 0}, {-1,-1, 0}, { 1,-1, 0}, {-1, 1, 0}, { 1, 0, 1}, {-1, 0,-1},
    { 1, 0,-1}, {-1, 0, 1}, { 0, 1, 1}, { 0,-1,-1}, { 0, 1,-1}, { 0,-1, 1}
};


}; // namespace LBM

#endif // MECHSYS_LBM_DOMAIN_H
