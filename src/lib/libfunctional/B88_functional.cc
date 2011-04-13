/**********************************************************
* B88_functional.cc: definitions for B88_functional for KS-DFT
* Robert Parrish, robparrish@gmail.com
* Autogenerated by MATLAB Script on 15-Sep-2010
*
***********************************************************/
#include <libmints/properties.h>
#include <libciomr/libciomr.h>
#include "B88_functional.h"
#include <stdlib.h>
#include <cmath>
#include <string>
#include <string>
#include <vector>

using namespace psi;
using namespace boost;
using namespace std;

namespace psi { namespace functional {

B88_Functional::B88_Functional(int npoints, int deriv) : Functional(npoints, deriv)
{
    
    name_ = "B88";
    description_ = "Becke 88 Exchange (GGA Only)";
    citation_ = "A.D. Becke, Phys. Rev. A, 38(6):3098-3100, 1988";
    
    double d = 4.1999999999999997E-03;
    params_.push_back(make_pair("d",d));

    is_gga_ = true;
    is_meta_ = false;

    //Required allocateion
    allocate();
}
B88_Functional::~B88_Functional()
{
}
void B88_Functional::computeRKSFunctional(boost::shared_ptr<Properties> prop)
{
    int ntrue = prop->getTrueSize();

    const double* rho_a;
    const double* gamma_aa;
    const double* tau_a;

    rho_a = prop->getRhoA();
    if (is_gga_) {
        gamma_aa = prop->getGammaAA();
    }
    if (is_meta_) {
        tau_a = prop->getTauA();
    }

    double d = params_[0].second;

    //Functional
    for (int index = 0; index < ntrue; index++) {

        //Functional Value
        if (rho_a[index] > cutoff_) {
            double t2681 = sqrt(gamma_aa[index]);
            functional_[index] = (d*gamma_aa[index]*pow(rho_a[index],4.0/3.0)*-2.0)/(d*t2681*log(t2681+sqrt(t2681* \
               t2681+1.0))*6.0+1.0);
        } else {
            functional_[index] = 0.0;
        } 

    }
    //First Partials
    for (int index = 0; index < ntrue && deriv_ >= 1; index++) {
        
        //V_rho_a
        if (rho_a[index] > cutoff_) {
            double t2683 = sqrt(gamma_aa[index]);
            v_rho_a_[index] = (d*gamma_aa[index]*pow(rho_a[index],1.0/3.0)*(-8.0/3.0))/(d*t2683*log(t2683+sqrt(t2683* \
               t2683+1.0))*6.0+1.0);
        } else {
            v_rho_a_[index] = 0.0;
        } 
        
        if (is_gga_) {
            
            if (rho_a[index] > cutoff_) {
                double t2685 = sqrt(gamma_aa[index]);
                double t2686 = pow(rho_a[index],4.0/3.0);
                double t2687 = log(t2685+sqrt(t2685*t2685+1.0));
                double t2688 = d*t2685*t2687*6.0;
                double t2689 = t2688+1.0;
                v_gamma_aa_[index] = (d*t2686*-2.0)/t2689+d*gamma_aa[index]*t2686*1/(t2689*t2689)*(d*1/sqrt(gamma_aa[index]+ \
                   1.0)*3.0+d*1/sqrt(gamma_aa[index])*t2687*3.0)*2.0;
            } else {
                v_gamma_aa_[index] = 0.0;
            } 
        
        }
        if (is_meta_) {
        
            //V_tau_a
            if (rho_a[index] > cutoff_) {
                v_tau_a_[index] = 0.0;
            } else {
                v_tau_a_[index] = 0.0;
            } 
        
        }
    }
    //Second Partials
    for (int index = 0; index < ntrue && deriv_ >= 2; index++) {
        
        //V_rho_a_rho_a
        if (rho_a[index] > cutoff_) {
            double t2692 = sqrt(gamma_aa[index]);
            v_rho_a_rho_a_[index] = (d*gamma_aa[index]*1/pow(rho_a[index],2.0/3.0)*(-8.0/9.0))/(d*t2692*log(t2692+ \
               sqrt(t2692*t2692+1.0))*6.0+1.0);
        } else {
            v_rho_a_rho_a_[index] = 0.0;
        } 
        
        if (is_gga_) {
        
            //V_rho_a_gamma_aa
            if (rho_a[index] > cutoff_) {
                double t2694 = sqrt(gamma_aa[index]);
                double t2695 = pow(rho_a[index],1.0/3.0);
                double t2696 = log(t2694+sqrt(t2694*t2694+1.0));
                double t2697 = d*t2694*t2696*6.0;
                double t2698 = t2697+1.0;
                v_rho_a_gamma_aa_[index] = (d*t2695*(-8.0/3.0))/t2698+d*gamma_aa[index]*t2695*1/(t2698*t2698)*(d* \
                   1/sqrt(gamma_aa[index]+1.0)*3.0+d*1/sqrt(gamma_aa[index])*t2696*3.0)*(8.0/3.0);
            } else {
                v_rho_a_gamma_aa_[index] = 0.0;
            } 
        
            //V_gamma_aa_gamma_aa
            if (rho_a[index] > cutoff_) {
                double t2700 = sqrt(gamma_aa[index]);
                double t2701 = log(t2700+sqrt(t2700*t2700+1.0));
                double t2702 = pow(rho_a[index],4.0/3.0);
                double t2703 = d*t2700*t2701*6.0;
                double t2704 = t2703+1.0;
                double t2705 = 1/(t2704*t2704);
                double t2706 = gamma_aa[index]+1.0;
                double t2707 = 1/sqrt(t2706);
                double t2708 = d*t2707*3.0;
                double t2709 = 1/sqrt(gamma_aa[index]);
                double t2710 = d*t2701*t2709*3.0;
                double t2711 = t2710+t2708;
                v_gamma_aa_gamma_aa_[index] = d*t2702*t2711*t2705*4.0-d*gamma_aa[index]*t2702*t2705*(d*1/pow(t2706,3.0/ \
                   2.0)*(3.0/2.0)+d*1/pow(gamma_aa[index],3.0/2.0)*t2701*(3.0/2.0)-(d*t2707*(3.0/2.0))/gamma_aa[index]) \
                   *2.0-d*gamma_aa[index]*t2702*(t2711*t2711)*1/(t2704*t2704*t2704)*4.0;
            } else {
                v_gamma_aa_gamma_aa_[index] = 0.0;
            } 
        
        }
        if (is_meta_) {
        
            //V_rho_a_tau_a
            if (rho_a[index] > cutoff_) {
                v_rho_a_tau_a_[index] = 0.0;
            } else {
                v_rho_a_tau_a_[index] = 0.0;
            } 
        
            //V_tau_a_tau_a
            if (rho_a[index] > cutoff_) {
                v_tau_a_tau_a_[index] = 0.0;
            } else {
                v_tau_a_tau_a_[index] = 0.0;
            } 
       
            if (is_gga_) {
        
                //V_gamma_aa_tau_a
                if (rho_a[index] > cutoff_) {
                    v_gamma_aa_tau_a_[index] = 0.0;
                } else {
                    v_gamma_aa_tau_a_[index] = 0.0;
                } 
       
            }
        }
    }
}
void B88_Functional::computeUKSFunctional(boost::shared_ptr<Properties> prop)
{
    int ntrue = prop->getTrueSize();

    const double* rho_a;
    const double* rho_b;
    const double* gamma_aa;
    const double* gamma_ab;
    const double* gamma_bb;
    const double* tau_a;
    const double* tau_b;

    rho_a = prop->getRhoA();
    rho_b = prop->getRhoB();
    if (is_gga_) {
        gamma_aa = prop->getGammaAA();
        gamma_ab = prop->getGammaAB();
        gamma_bb = prop->getGammaBB();
    }
    if (is_meta_) {
        tau_a = prop->getTauA();
        tau_b = prop->getTauB();
    }

    double d = params_[0].second;
    
    //Functional
    for (int index = 0; index < ntrue; index++) {

        //Functional Value
        if (rho_a[index] > cutoff_ && rho_b[index] > cutoff_) {
            double t2437 = sqrt(gamma_aa[index]);
            double t2438 = sqrt(gamma_bb[index]);
            functional_[index] = -(d*gamma_aa[index]*pow(rho_a[index],4.0/3.0))/(d*t2437*log(t2437+sqrt(t2437* \
               t2437+1.0))*6.0+1.0)-(d*gamma_bb[index]*pow(rho_b[index],4.0/3.0))/(d*t2438*log(t2438+sqrt(t2438*t2438+ \
               1.0))*6.0+1.0);
        } else if (rho_a[index] > cutoff_) {
            double t2584 = sqrt(gamma_aa[index]);
            functional_[index] = -(d*gamma_aa[index]*pow(rho_a[index],4.0/3.0))/(d*t2584*log(t2584+sqrt(t2584* \
               t2584+1.0))*6.0+1.0);
        } else if (rho_b[index] > cutoff_) {
            double t2523 = sqrt(gamma_bb[index]);
            functional_[index] = -(d*gamma_bb[index]*pow(rho_b[index],4.0/3.0))/(d*t2523*log(t2523+sqrt(t2523* \
               t2523+1.0))*6.0+1.0);
        } else {
            functional_[index] = 0.0;
        } 
   
    }
    //First Partials
    for (int index = 0; index < ntrue && deriv_ >= 1; index++) {

        //V_rho_a
        if (rho_a[index] > cutoff_ && rho_b[index] > cutoff_) {
            double t2440 = sqrt(gamma_aa[index]);
            v_rho_a_[index] = (d*gamma_aa[index]*pow(rho_a[index],1.0/3.0)*(-4.0/3.0))/(d*t2440*log(t2440+sqrt(t2440* \
               t2440+1.0))*6.0+1.0);
        } else if (rho_a[index] > cutoff_) {
            double t2586 = sqrt(gamma_aa[index]);
            v_rho_a_[index] = (d*gamma_aa[index]*pow(rho_a[index],1.0/3.0)*(-4.0/3.0))/(d*t2586*log(t2586+sqrt(t2586* \
               t2586+1.0))*6.0+1.0);
        } else if (rho_b[index] > cutoff_) {
            v_rho_a_[index] = 0.0;
        } else {
            v_rho_a_[index] = 0.0;
        } 

        //V_rho_b
        if (rho_a[index] > cutoff_ && rho_b[index] > cutoff_) {
            double t2442 = sqrt(gamma_bb[index]);
            v_rho_b_[index] = (d*gamma_bb[index]*pow(rho_b[index],1.0/3.0)*(-4.0/3.0))/(d*t2442*log(t2442+sqrt(t2442* \
               t2442+1.0))*6.0+1.0);
        } else if (rho_a[index] > cutoff_) {
            v_rho_b_[index] = 0.0;
        } else if (rho_b[index] > cutoff_) {
            double t2526 = sqrt(gamma_bb[index]);
            v_rho_b_[index] = (d*gamma_bb[index]*pow(rho_b[index],1.0/3.0)*(-4.0/3.0))/(d*t2526*log(t2526+sqrt(t2526* \
               t2526+1.0))*6.0+1.0);
        } else {
            v_rho_b_[index] = 0.0;
        } 

        if (is_gga_) {

            //V_gamma_aa
            if (rho_a[index] > cutoff_ && rho_b[index] > cutoff_) {
                double t2444 = sqrt(gamma_aa[index]);
                double t2445 = pow(rho_a[index],4.0/3.0);
                double t2446 = log(t2444+sqrt(t2444*t2444+1.0));
                double t2447 = d*t2444*t2446*6.0;
                double t2448 = t2447+1.0;
                v_gamma_aa_[index] = -(d*t2445)/t2448+d*gamma_aa[index]*t2445*1/(t2448*t2448)*(d*1/sqrt(gamma_aa[index]+ \
                   1.0)*3.0+d*1/sqrt(gamma_aa[index])*t2446*3.0);
            } else if (rho_a[index] > cutoff_) {
                double t2589 = sqrt(gamma_aa[index]);
                double t2590 = pow(rho_a[index],4.0/3.0);
                double t2591 = log(t2589+sqrt(t2589*t2589+1.0));
                double t2592 = d*t2591*t2589*6.0;
                double t2593 = t2592+1.0;
                v_gamma_aa_[index] = -(d*t2590)/t2593+d*gamma_aa[index]*t2590*1/(t2593*t2593)*(d*1/sqrt(gamma_aa[index]+ \
                   1.0)*3.0+d*1/sqrt(gamma_aa[index])*t2591*3.0);
            } else if (rho_b[index] > cutoff_) {
                v_gamma_aa_[index] = 0.0;
            } else {
                v_gamma_aa_[index] = 0.0;
            } 

            //V_gamma_ab
            if (rho_a[index] > cutoff_ && rho_b[index] > cutoff_) {
                v_gamma_ab_[index] = 0.0;
            } else if (rho_a[index] > cutoff_) {
                v_gamma_ab_[index] = 0.0;
            } else if (rho_b[index] > cutoff_) {
                v_gamma_ab_[index] = 0.0;
            } else {
                v_gamma_ab_[index] = 0.0;
            } 

            //V_gamma_bb
            if (rho_a[index] > cutoff_ && rho_b[index] > cutoff_) {
                double t2451 = sqrt(gamma_bb[index]);
                double t2452 = pow(rho_b[index],4.0/3.0);
                double t2453 = log(t2451+sqrt(t2451*t2451+1.0));
                double t2454 = d*t2451*t2453*6.0;
                double t2455 = t2454+1.0;
                v_gamma_bb_[index] = -(d*t2452)/t2455+d*gamma_bb[index]*t2452*1/(t2455*t2455)*(d*1/sqrt(gamma_bb[index]+ \
                   1.0)*3.0+d*1/sqrt(gamma_bb[index])*t2453*3.0);
            } else if (rho_a[index] > cutoff_) {
                v_gamma_bb_[index] = 0.0;
            } else if (rho_b[index] > cutoff_) {
                double t2530 = sqrt(gamma_bb[index]);
                double t2531 = pow(rho_b[index],4.0/3.0);
                double t2532 = log(t2530+sqrt(t2530*t2530+1.0));
                double t2533 = d*t2530*t2532*6.0;
                double t2534 = t2533+1.0;
                v_gamma_bb_[index] = -(d*t2531)/t2534+d*gamma_bb[index]*t2531*1/(t2534*t2534)*(d*1/sqrt(gamma_bb[index]+ \
                   1.0)*3.0+d*1/sqrt(gamma_bb[index])*t2532*3.0);
            } else {
                v_gamma_bb_[index] = 0.0;
            } 
        }
        if (is_meta_) {

            //V_tau_a
            if (rho_a[index] > cutoff_ && rho_b[index] > cutoff_) {
                v_tau_a_[index] = 0.0;
            } else if (rho_a[index] > cutoff_) {
                v_tau_a_[index] = 0.0;
            } else if (rho_b[index] > cutoff_) {
                v_tau_a_[index] = 0.0;
            } else {
                v_tau_a_[index] = 0.0;
            } 

            //V_tau_a
            if (rho_a[index] > cutoff_ && rho_b[index] > cutoff_) {
                v_tau_b_[index] = 0.0;
            } else if (rho_a[index] > cutoff_) {
                v_tau_b_[index] = 0.0;
            } else if (rho_b[index] > cutoff_) {
                v_tau_b_[index] = 0.0;
            } else {
                v_tau_b_[index] = 0.0;
            } 
        }
    }
    //Second Partials
    for (int index = 0; index < ntrue && deriv_ >= 2; index++) {

        //V_rho_a_rho_a
        if (rho_a[index] > cutoff_ && rho_b[index] > cutoff_) {
            double t2459 = sqrt(gamma_aa[index]);
            v_rho_a_rho_a_[index] = (d*gamma_aa[index]*1/pow(rho_a[index],2.0/3.0)*(-4.0/9.0))/(d*t2459*log(t2459+ \
               sqrt(t2459*t2459+1.0))*6.0+1.0);
        } else if (rho_a[index] > cutoff_) {
            double t2599 = sqrt(gamma_aa[index]);
            v_rho_a_rho_a_[index] = (d*gamma_aa[index]*1/pow(rho_a[index],2.0/3.0)*(-4.0/9.0))/(d*t2599*log(t2599+ \
               sqrt(t2599*t2599+1.0))*6.0+1.0);
        } else if (rho_b[index] > cutoff_) {
            v_rho_a_rho_a_[index] = 0.0;
        } else {
            v_rho_a_rho_a_[index] = 0.0;
        } 

        //V_rho_a_rho_b
        if (rho_a[index] > cutoff_ && rho_b[index] > cutoff_) {
            v_rho_a_rho_b_[index] = 0.0;
        } else if (rho_a[index] > cutoff_) {
            v_rho_a_rho_b_[index] = 0.0;
        } else if (rho_b[index] > cutoff_) {
            v_rho_a_rho_b_[index] = 0.0;
        } else {
            v_rho_a_rho_b_[index] = 0.0;
        } 

        //V_rho_b_rho_b
        if (rho_a[index] > cutoff_ && rho_b[index] > cutoff_) {
            double t2462 = sqrt(gamma_bb[index]);
            v_rho_b_rho_b_[index] = (d*gamma_bb[index]*1/pow(rho_b[index],2.0/3.0)*(-4.0/9.0))/(d*t2462*log(t2462+ \
               sqrt(t2462*t2462+1.0))*6.0+1.0);
        } else if (rho_a[index] > cutoff_) {
            v_rho_b_rho_b_[index] = 0.0;
        } else if (rho_b[index] > cutoff_) {
            double t2540 = sqrt(gamma_bb[index]);
            v_rho_b_rho_b_[index] = (d*gamma_bb[index]*1/pow(rho_b[index],2.0/3.0)*(-4.0/9.0))/(d*t2540*log(t2540+ \
               sqrt(t2540*t2540+1.0))*6.0+1.0);
        } else {
            v_rho_b_rho_b_[index] = 0.0;
        } 

        if (is_gga_) {

            //V_rho_a_gamma_aa
            if (rho_a[index] > cutoff_ && rho_b[index] > cutoff_) {
                double t2464 = sqrt(gamma_aa[index]);
                double t2465 = pow(rho_a[index],1.0/3.0);
                double t2466 = log(t2464+sqrt(t2464*t2464+1.0));
                double t2467 = d*t2464*t2466*6.0;
                double t2468 = t2467+1.0;
                v_rho_a_gamma_aa_[index] = (d*t2465*(-4.0/3.0))/t2468+d*gamma_aa[index]*t2465*1/(t2468*t2468)*(d* \
                   1/sqrt(gamma_aa[index]+1.0)*3.0+d*1/sqrt(gamma_aa[index])*t2466*3.0)*(4.0/3.0);
            } else if (rho_a[index] > cutoff_) {
                double t2603 = sqrt(gamma_aa[index]);
                double t2604 = pow(rho_a[index],1.0/3.0);
                double t2605 = log(t2603+sqrt(t2603*t2603+1.0));
                double t2606 = d*t2603*t2605*6.0;
                double t2607 = t2606+1.0;
                v_rho_a_gamma_aa_[index] = (d*t2604*(-4.0/3.0))/t2607+d*gamma_aa[index]*t2604*1/(t2607*t2607)*(d* \
                   1/sqrt(gamma_aa[index]+1.0)*3.0+d*1/sqrt(gamma_aa[index])*t2605*3.0)*(4.0/3.0);
            } else if (rho_b[index] > cutoff_) {
                v_rho_a_gamma_aa_[index] = 0.0;
            } else {
                v_rho_a_gamma_aa_[index] = 0.0;
            } 

            //V_rho_a_gamma_ab
            if (rho_a[index] > cutoff_ && rho_b[index] > cutoff_) {
                v_rho_a_gamma_ab_[index] = 0.0;
            } else if (rho_a[index] > cutoff_) {
                v_rho_a_gamma_ab_[index] = 0.0;
            } else if (rho_b[index] > cutoff_) {
                v_rho_a_gamma_ab_[index] = 0.0;
            } else {
                v_rho_a_gamma_ab_[index] = 0.0;
            } 

            //V_rho_a_gamma_bb
            if (rho_a[index] > cutoff_ && rho_b[index] > cutoff_) {
                v_rho_a_gamma_bb_[index] = 0.0;
            } else if (rho_a[index] > cutoff_) {
                v_rho_a_gamma_bb_[index] = 0.0;
            } else if (rho_b[index] > cutoff_) {
                v_rho_a_gamma_bb_[index] = 0.0;
            } else {
                v_rho_a_gamma_bb_[index] = 0.0;
            } 

            //V_rho_b_gamma_aa
            if (rho_a[index] > cutoff_ && rho_b[index] > cutoff_) {
                v_rho_b_gamma_aa_[index] = 0.0;
            } else if (rho_a[index] > cutoff_) {
                v_rho_b_gamma_aa_[index] = 0.0;
            } else if (rho_b[index] > cutoff_) {
                v_rho_b_gamma_aa_[index] = 0.0;
            } else {
                v_rho_b_gamma_aa_[index] = 0.0;
            } 

            //V_rho_b_gamma_ab
            if (rho_a[index] > cutoff_ && rho_b[index] > cutoff_) {
                v_rho_b_gamma_ab_[index] = 0.0;
            } else if (rho_a[index] > cutoff_) {
                v_rho_b_gamma_ab_[index] = 0.0;
            } else if (rho_b[index] > cutoff_) {
                v_rho_b_gamma_ab_[index] = 0.0;
            } else {
                v_rho_b_gamma_ab_[index] = 0.0;
            } 

            //V_rho_b_gamma_bb
            if (rho_a[index] > cutoff_ && rho_b[index] > cutoff_) {
                double t2474 = sqrt(gamma_bb[index]);
                double t2475 = pow(rho_b[index],1.0/3.0);
                double t2476 = log(t2474+sqrt(t2474*t2474+1.0));
                double t2477 = d*t2474*t2476*6.0;
                double t2478 = t2477+1.0;
                v_rho_b_gamma_bb_[index] = (d*t2475*(-4.0/3.0))/t2478+d*gamma_bb[index]*t2475*1/(t2478*t2478)*(d* \
                   1/sqrt(gamma_bb[index]+1.0)*3.0+d*1/sqrt(gamma_bb[index])*t2476*3.0)*(4.0/3.0);
            } else if (rho_a[index] > cutoff_) {
                v_rho_b_gamma_bb_[index] = 0.0;
            } else if (rho_b[index] > cutoff_) {
                double t2547 = sqrt(gamma_bb[index]);
                double t2548 = pow(rho_b[index],1.0/3.0);
                double t2549 = log(t2547+sqrt(t2547*t2547+1.0));
                double t2550 = d*t2547*t2549*6.0;
                double t2551 = t2550+1.0;
                v_rho_b_gamma_bb_[index] = (d*t2548*(-4.0/3.0))/t2551+d*gamma_bb[index]*1/(t2551*t2551)*t2548*(d* \
                   1/sqrt(gamma_bb[index]+1.0)*3.0+d*1/sqrt(gamma_bb[index])*t2549*3.0)*(4.0/3.0);
            } else {
                v_rho_b_gamma_bb_[index] = 0.0;
            } 

            //V_gamma_aa_gamma_aa
            if (rho_a[index] > cutoff_ && rho_b[index] > cutoff_) {
                double t2480 = sqrt(gamma_aa[index]);
                double t2481 = log(t2480+sqrt(t2480*t2480+1.0));
                double t2482 = pow(rho_a[index],4.0/3.0);
                double t2483 = d*t2480*t2481*6.0;
                double t2484 = t2483+1.0;
                double t2485 = 1/(t2484*t2484);
                double t2486 = gamma_aa[index]+1.0;
                double t2487 = 1/sqrt(t2486);
                double t2488 = d*t2487*3.0;
                double t2489 = 1/sqrt(gamma_aa[index]);
                double t2490 = d*t2481*t2489*3.0;
                double t2491 = t2490+t2488;
                v_gamma_aa_gamma_aa_[index] = d*t2482*t2491*t2485*2.0-d*gamma_aa[index]*t2482*t2485*(d*1/pow(t2486,3.0/ \
                   2.0)*(3.0/2.0)+d*1/pow(gamma_aa[index],3.0/2.0)*t2481*(3.0/2.0)-(d*t2487*(3.0/2.0))/gamma_aa[index]) \
                   -d*gamma_aa[index]*t2482*(t2491*t2491)*1/(t2484*t2484*t2484)*2.0;
            } else if (rho_a[index] > cutoff_) {
                double t2614 = sqrt(gamma_aa[index]);
                double t2615 = log(t2614+sqrt(t2614*t2614+1.0));
                double t2616 = pow(rho_a[index],4.0/3.0);
                double t2617 = d*t2614*t2615*6.0;
                double t2618 = t2617+1.0;
                double t2619 = 1/(t2618*t2618);
                double t2620 = gamma_aa[index]+1.0;
                double t2621 = 1/sqrt(t2620);
                double t2622 = d*t2621*3.0;
                double t2623 = 1/sqrt(gamma_aa[index]);
                double t2624 = d*t2623*t2615*3.0;
                double t2625 = t2622+t2624;
                v_gamma_aa_gamma_aa_[index] = d*t2616*t2625*t2619*2.0-d*gamma_aa[index]*t2616*t2619*(d*1/pow(t2620,3.0/ \
                   2.0)*(3.0/2.0)-(d*t2621*(3.0/2.0))/gamma_aa[index]+d*1/pow(gamma_aa[index],3.0/2.0)*t2615*(3.0/2.0) \
                   )-d*gamma_aa[index]*t2616*(t2625*t2625)*1/(t2618*t2618*t2618)*2.0;
            } else if (rho_b[index] > cutoff_) {
                v_gamma_aa_gamma_aa_[index] = 0.0;
            } else {
                v_gamma_aa_gamma_aa_[index] = 0.0;
            } 

            //V_gamma_aa_gamma_ab
            if (rho_a[index] > cutoff_ && rho_b[index] > cutoff_) {
                v_gamma_aa_gamma_ab_[index] = 0.0;
            } else if (rho_a[index] > cutoff_) {
                v_gamma_aa_gamma_ab_[index] = 0.0;
            } else if (rho_b[index] > cutoff_) {
                v_gamma_aa_gamma_ab_[index] = 0.0;
            } else {
                v_gamma_aa_gamma_ab_[index] = 0.0;
            } 

            //V_gamma_aa_gamma_bb
            if (rho_a[index] > cutoff_ && rho_b[index] > cutoff_) {
                v_gamma_aa_gamma_bb_[index] = 0.0;
            } else if (rho_a[index] > cutoff_) {
                v_gamma_aa_gamma_bb_[index] = 0.0;
            } else if (rho_b[index] > cutoff_) {
                v_gamma_aa_gamma_bb_[index] = 0.0;
            } else {
                v_gamma_aa_gamma_bb_[index] = 0.0;
            } 

            //V_gamma_ab_gamma_ab
            if (rho_a[index] > cutoff_ && rho_b[index] > cutoff_) {
                v_gamma_ab_gamma_ab_[index] = 0.0;
            } else if (rho_a[index] > cutoff_) {
                v_gamma_ab_gamma_ab_[index] = 0.0;
            } else if (rho_b[index] > cutoff_) {
                v_gamma_ab_gamma_ab_[index] = 0.0;
            } else {
                v_gamma_ab_gamma_ab_[index] = 0.0;
            } 

            //V_gamma_ab_gamma_bb
            if (rho_a[index] > cutoff_ && rho_b[index] > cutoff_) {
                v_gamma_ab_gamma_bb_[index] = 0.0;
            } else if (rho_a[index] > cutoff_) {
                v_gamma_ab_gamma_bb_[index] = 0.0;
            } else if (rho_b[index] > cutoff_) {
                v_gamma_ab_gamma_bb_[index] = 0.0;
            } else {
                v_gamma_ab_gamma_bb_[index] = 0.0;
            } 

            //V_gamma_bb_gamma_bb
            if (rho_a[index] > cutoff_ && rho_b[index] > cutoff_) {
                double t2497 = sqrt(gamma_bb[index]);
                double t2498 = log(t2497+sqrt(t2497*t2497+1.0));
                double t2499 = pow(rho_b[index],4.0/3.0);
                double t2500 = d*t2497*t2498*6.0;
                double t2501 = t2500+1.0;
                double t2502 = 1/(t2501*t2501);
                double t2503 = gamma_bb[index]+1.0;
                double t2504 = 1/sqrt(t2503);
                double t2505 = d*t2504*3.0;
                double t2506 = 1/sqrt(gamma_bb[index]);
                double t2507 = d*t2498*t2506*3.0;
                double t2508 = t2505+t2507;
                v_gamma_bb_gamma_bb_[index] = d*t2499*t2502*t2508*2.0-d*gamma_bb[index]*t2499*t2502*(d*1/pow(t2503,3.0/ \
                   2.0)*(3.0/2.0)+d*1/pow(gamma_bb[index],3.0/2.0)*t2498*(3.0/2.0)-(d*t2504*(3.0/2.0))/gamma_bb[index]) \
                   -d*gamma_bb[index]*t2499*1/(t2501*t2501*t2501)*(t2508*t2508)*2.0;
            } else if (rho_a[index] > cutoff_) {
                v_gamma_bb_gamma_bb_[index] = 0.0;
            } else if (rho_b[index] > cutoff_) {
                double t2558 = sqrt(gamma_bb[index]);
                double t2559 = log(t2558+sqrt(t2558*t2558+1.0));
                double t2560 = pow(rho_b[index],4.0/3.0);
                double t2561 = d*t2558*t2559*6.0;
                double t2562 = t2561+1.0;
                double t2563 = 1/(t2562*t2562);
                double t2564 = gamma_bb[index]+1.0;
                double t2565 = 1/sqrt(t2564);
                double t2566 = d*t2565*3.0;
                double t2567 = 1/sqrt(gamma_bb[index]);
                double t2568 = d*t2567*t2559*3.0;
                double t2569 = t2566+t2568;
                v_gamma_bb_gamma_bb_[index] = d*t2560*t2563*t2569*2.0-d*gamma_bb[index]*t2560*t2563*(d*1/pow(t2564,3.0/ \
                   2.0)*(3.0/2.0)-(d*t2565*(3.0/2.0))/gamma_bb[index]+d*1/pow(gamma_bb[index],3.0/2.0)*t2559*(3.0/2.0) \
                   )-d*gamma_bb[index]*t2560*1/(t2562*t2562*t2562)*(t2569*t2569)*2.0;
            } else {
                v_gamma_bb_gamma_bb_[index] = 0.0;
            } 

        }                       
        if (is_meta_) {

            //V_rho_a_tau_a
            if (rho_a[index] > cutoff_ && rho_b[index] > cutoff_) {
                v_rho_a_tau_a_[index] = 0.0;
            } else if (rho_a[index] > cutoff_) {
                v_rho_a_tau_a_[index] = 0.0;
            } else if (rho_b[index] > cutoff_) {
                v_rho_a_tau_a_[index] = 0.0;
            } else {
                v_rho_a_tau_a_[index] = 0.0;
            } 

            //V_rho_a_tau_b
            if (rho_a[index] > cutoff_ && rho_b[index] > cutoff_) {
                v_rho_a_tau_b_[index] = 0.0;
            } else if (rho_a[index] > cutoff_) {
                v_rho_a_tau_b_[index] = 0.0;
            } else if (rho_b[index] > cutoff_) {
                v_rho_a_tau_b_[index] = 0.0;
            } else {
                v_rho_a_tau_b_[index] = 0.0;
            } 

            //V_rho_b_tau_a
            if (rho_a[index] > cutoff_ && rho_b[index] > cutoff_) {
                v_rho_b_tau_a_[index] = 0.0;
            } else if (rho_a[index] > cutoff_) {
                v_rho_b_tau_a_[index] = 0.0;
            } else if (rho_b[index] > cutoff_) {
                v_rho_b_tau_a_[index] = 0.0;
            } else {
                v_rho_b_tau_a_[index] = 0.0;
            } 

            //V_rho_b_tau_b
            if (rho_a[index] > cutoff_ && rho_b[index] > cutoff_) {
                v_rho_b_tau_b_[index] = 0.0;
            } else if (rho_a[index] > cutoff_) {
                v_rho_b_tau_b_[index] = 0.0;
            } else if (rho_b[index] > cutoff_) {
                v_rho_b_tau_b_[index] = 0.0;
            } else {
                v_rho_b_tau_b_[index] = 0.0;
            } 

            //V_tau_a_tau_a
            if (rho_a[index] > cutoff_ && rho_b[index] > cutoff_) {
                v_tau_a_tau_a_[index] = 0.0;
            } else if (rho_a[index] > cutoff_) {
                v_tau_a_tau_a_[index] = 0.0;
            } else if (rho_b[index] > cutoff_) {
                v_tau_a_tau_a_[index] = 0.0;
            } else {
                v_tau_a_tau_a_[index] = 0.0;
            } 

            //V_tau_a_tau_b
            if (rho_a[index] > cutoff_ && rho_b[index] > cutoff_) {
                v_tau_a_tau_b_[index] = 0.0;
            } else if (rho_a[index] > cutoff_) {
                v_tau_a_tau_b_[index] = 0.0;
            } else if (rho_b[index] > cutoff_) {
                v_tau_a_tau_b_[index] = 0.0;
            } else {
                v_tau_a_tau_b_[index] = 0.0;
            } 

            //V_tau_b_tau_b
            if (rho_a[index] > cutoff_ && rho_b[index] > cutoff_) {
                v_tau_b_tau_b_[index] = 0.0;
            } else if (rho_a[index] > cutoff_) {
                v_tau_b_tau_b_[index] = 0.0;
            } else if (rho_b[index] > cutoff_) {
                v_tau_b_tau_b_[index] = 0.0;
            } else {
                v_tau_b_tau_b_[index] = 0.0;
            } 

            if (is_gga_) {
            	
                //V_gamma_aa_tau_a
                if (rho_a[index] > cutoff_ && rho_b[index] > cutoff_) {
                    v_gamma_aa_tau_a_[index] = 0.0;
                } else if (rho_a[index] > cutoff_) {
                    v_gamma_aa_tau_a_[index] = 0.0;
                } else if (rho_b[index] > cutoff_) {
                    v_gamma_aa_tau_a_[index] = 0.0;
                } else {
                    v_gamma_aa_tau_a_[index] = 0.0;
                } 
            	
                //V_gamma_aa_tau_b
                if (rho_a[index] > cutoff_ && rho_b[index] > cutoff_) {
                    v_gamma_aa_tau_b_[index] = 0.0;
                } else if (rho_a[index] > cutoff_) {
                    v_gamma_aa_tau_b_[index] = 0.0;
                } else if (rho_b[index] > cutoff_) {
                    v_gamma_aa_tau_b_[index] = 0.0;
                } else {
                    v_gamma_aa_tau_b_[index] = 0.0;
                } 
            	
                //V_gamma_ab_tau_a
                if (rho_a[index] > cutoff_ && rho_b[index] > cutoff_) {
                    v_gamma_ab_tau_a_[index] = 0.0;
                } else if (rho_a[index] > cutoff_) {
                    v_gamma_ab_tau_a_[index] = 0.0;
                } else if (rho_b[index] > cutoff_) {
                    v_gamma_ab_tau_a_[index] = 0.0;
                } else {
                    v_gamma_ab_tau_a_[index] = 0.0;
                } 
            	
                //V_gamma_ab_tau_b
                if (rho_a[index] > cutoff_ && rho_b[index] > cutoff_) {
                    v_gamma_ab_tau_b_[index] = 0.0;
                } else if (rho_a[index] > cutoff_) {
                    v_gamma_ab_tau_b_[index] = 0.0;
                } else if (rho_b[index] > cutoff_) {
                    v_gamma_ab_tau_b_[index] = 0.0;
                } else {
                    v_gamma_ab_tau_b_[index] = 0.0;
                } 
            	
                //V_gamma_bb_tau_a
                if (rho_a[index] > cutoff_ && rho_b[index] > cutoff_) {
                    v_gamma_bb_tau_a_[index] = 0.0;
                } else if (rho_a[index] > cutoff_) {
                    v_gamma_bb_tau_a_[index] = 0.0;
                } else if (rho_b[index] > cutoff_) {
                    v_gamma_bb_tau_a_[index] = 0.0;
                } else {
                    v_gamma_bb_tau_a_[index] = 0.0;
                } 
            	
                //V_gamma_bb_tau_b
                if (rho_a[index] > cutoff_ && rho_b[index] > cutoff_) {
                    v_gamma_bb_tau_b_[index] = 0.0;
                } else if (rho_a[index] > cutoff_) {
                    v_gamma_bb_tau_b_[index] = 0.0;
                } else if (rho_b[index] > cutoff_) {
                    v_gamma_bb_tau_b_[index] = 0.0;
                } else {
                    v_gamma_bb_tau_b_[index] = 0.0;
                } 
          
            }  
        }
    }
}

}}

