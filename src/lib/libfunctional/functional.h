/*
 * @BEGIN LICENSE
 *
 * Psi4: an open-source quantum chemistry software package
 *
 * Copyright (c) 2007-2016 The Psi4 Developers.
 *
 * The copyrights for code used from other parties are included in
 * the corresponding files.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * @END LICENSE
 */

#ifndef FUNCTIONAL_H
#define FUNCTIONAL_H

#include <libmints/typedefs.h>
#include <map>
#include <vector>

namespace psi {

/** 
 * Functional: Generic Semilocal Exchange or Correlation DFA functional
 * 
 * A DFT functional is defined as:
 * 
 * E_XC = E_X + E_C
 * E_X  = (1-\alpha_x) E_X^DFA [\omega_x] + \alpha E_X^HF + (1-\alpha) E_X^HF,LR
 * E_C  = (1-\alpha_c) E_C^DFA [\omega_c] + \alpha E_C^MP2 + (1-\alpha) E_C^MP2,LR
 * 
 **/
class Functional {

protected:

    // => Meta-Data <= //

    // Actually (1-\alpha)*w, the final scale of all computed values
    double alpha_; 
    // Omega, defaults to zero, throws if set for non-omega functionals
    double omega_;

    // Name of the functional (shorthand)
    std::string name_;
    // Description of functional
    std::string description_;
    // Citations(s) defining functionals 
    std::string citation_;
    
    // Is GGA?
    bool gga_;
    // Is Meta?
    bool meta_;
    // Is LRC?
    bool lrc_;

    // Parameter set
    std::map<std::string, double> parameters_;

    // Densty-based cutoff
    double lsda_cutoff_;
    // Tau-based cutoff
    double meta_cutoff_;

    // Initialize null functional
    void common_init();

public:

    // => Constructors (Use the factory constructor, or really know what's up) <= //

    Functional();
    virtual ~Functional(); 

    // Build a base version of a DFA functional (say B97_X)
    static boost::shared_ptr<Functional> build_base(const std::string& alias);
        
    // => Computers <= //
    
    virtual void compute_functional(const std::map<std::string,SharedVector>& in, const std::map<std::string,SharedVector>& out, int npoints, int deriv, double alpha) = 0;

    // => Parameters <= //
    
    const std::map<std::string, double>& parameters() { return parameters_; }
    virtual void set_parameter(const std::string& key, double val);

    // => Setters <= //

    void set_gga(bool gga) { gga_ = gga; }
    void set_meta(bool meta) { meta_ = meta; }
    void set_alpha(double alpha) { alpha_ = alpha; }
    void set_omega(double omega) { omega_ = omega; lrc_ = (omega_ != 0.0); }
    void set_name(const std::string & name) { name_ = name; }
    void set_description(const std::string & description) { description_ = description; }
    void set_citation(const std::string & citation) { citation_ = citation; }

    void set_lsda_cutoff(double cut) { lsda_cutoff_ = cut; }
    void set_meta_cutoff(double cut) { meta_cutoff_ = cut; }

    // => Accessors <= //

    std::string name() const { return name_; }
    std::string description() const { return description_; }
    std::string citation() const { return citation_; }
    
    bool is_meta() const { return meta_; }
    bool is_gga() const { return gga_; }
    bool is_lrc() const { return lrc_; }

    double alpha() const { return alpha_; }
    double omega() const { return omega_; }

    double lsda_cutoff() const { return lsda_cutoff_; }
    double meta_cutoff() const { return meta_cutoff_; }

    // => Utility <= //
    virtual void print(std::string OutFileRMR = "outfile", int print = 1) const;
    void py_print() const { print("outfile", 1); }
    void py_print_detail(int level) const { print("outfile", level); }

};

}

#endif