#include <cstdlib>
#include <cstdio>
#include <cmath>

#include <liboptions/liboptions.h>
#include <libparallel/parallel.h>
#include <psifiles.h>
#include <libciomr/libciomr.h>
#include <libpsio/psio.h>
#include <libchkpt/chkpt.h>
#include <libiwl/iwl.h>
#include <libqt/qt.h>
#include <libparallel/parallel.h>
#include <libmints/view.h>

#include "mints.h"

#include <psi4-dec.h>

#include <boost/python.hpp>
#include <boost/python/call.hpp>

#include <algorithm>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>

using namespace boost;
using namespace psi;

// Globals
size_t ioff[MAX_IOFF];
double df[MAX_DF];
double bc[MAX_BC][MAX_BC];
double fac[MAX_FAC];

Wavefunction::Wavefunction(Options & options, boost::shared_ptr<PSIO> psio) :
    options_(options), psio_(psio)
{
    chkpt_ = boost::shared_ptr<Chkpt>(new Chkpt(psio.get(), PSIO_OPEN_OLD));
    common_init();
}

Wavefunction::Wavefunction(Options & options, boost::shared_ptr<PSIO> psio, boost::shared_ptr<Chkpt> chkpt) :
    options_(options), psio_(psio), chkpt_(chkpt)
{
    common_init();
}

Wavefunction::~Wavefunction()
{
}

void Wavefunction::copy(boost::shared_ptr<Wavefunction> other) 
{
    name_ = other->name_;
    basisset_ = other->basisset_;
    sobasisset_ = other->sobasisset_;
    AO2SO_ = other->AO2SO_;
    S_ = other->S_;
    molecule_ = other->molecule_;
    psio_ = other->psio_;
    chkpt_ = other->chkpt_;
    integral_ = other->integral_;  
    factory_ = other->factory_;
    reference_wavefunction_ = other; 
    memory_ = other->memory_;
    debug_ = other->debug_;
    density_fitted_ = other->density_fitted_;
    energy_threshold_ = other->energy_threshold_;   
    density_threshold_ = other->density_threshold_;   
    nalpha_ = other->nalpha_;
    nbeta_ = other->nbeta_;
    
    doccpi_ = other->doccpi_;
    soccpi_ = other->soccpi_;
    frzcpi_ = other->frzcpi_;
    frzvpi_ = other->frzvpi_;
    nalphapi_ = other->nalphapi_;
    nbetapi_ = other->nbetapi_;
    nsopi_ = other->nsopi_;
    nmopi_ = other->nmopi_;

    energy_ = other->energy_;
    
    nso_ = other->nso_;
    nmo_ = other->nmo_;
    nirrep_ = other->nirrep_; 

    Ca_ = other->Ca_;    
    Cb_ = other->Cb_;    
    Da_ = other->Da_;    
    Db_ = other->Db_;    
    Fa_ = other->Fa_;    
    Fb_ = other->Fb_;    
    epsilon_a_ = other->epsilon_a_;    
    epsilon_b_ = other->epsilon_b_;    

    gradient_ = other->gradient_;
    tpdm_gradient_contribution_ = other->tpdm_gradient_contribution_;
}

void Wavefunction::common_init()
{
    Wavefunction::initialize_singletons();

    // Take the molecule from the environment
    molecule_ = Process::environment.molecule();

    // Load in the basis set
    boost::shared_ptr<BasisSetParser> parser(new Gaussian94BasisSetParser());
    basisset_ = BasisSet::construct(parser, molecule_, "BASIS");

    // Check the point group of the molecule. If it is not set, set it.
    if (!molecule_->point_group()) {
        molecule_->set_point_group(molecule_->find_point_group());
    }

    // Create an SO basis...we need the point group for this part.
    integral_ = boost::shared_ptr<IntegralFactory>(new IntegralFactory(basisset_, basisset_, basisset_, basisset_));
    sobasisset_ = boost::shared_ptr<SOBasisSet>(new SOBasisSet(basisset_, integral_));

    boost::shared_ptr<PetiteList> pet(new PetiteList(basisset_, integral_));
    AO2SO_ = pet->aotoso();

    // Obtain the dimension object to initialize the factory.
    const Dimension dimension = sobasisset_->dimension();
    factory_ = boost::shared_ptr<MatrixFactory>(new MatrixFactory);
    factory_->init_with(dimension, dimension);

    nirrep_ = dimension.n();

    S_ = factory_->create_shared_matrix("S");
    boost::shared_ptr<OneBodySOInt> Sint(integral_->so_overlap());
    Sint->compute(S_);

    // Initialize array that hold dimensionality information
    nsopi_    = Dimension(nirrep_, "SOs per irrep");
    nmopi_    = Dimension(nirrep_, "MOs per irrep");
    nalphapi_ = Dimension(nirrep_, "Alpha electrons per irrep");
    nbetapi_  = Dimension(nirrep_, "Beta electrons per irrep");
    doccpi_   = Dimension(nirrep_, "Doubly occupied orbitals per irrep");
    soccpi_   = Dimension(nirrep_, "Singly occupied orbitals per irrep");
    frzcpi_   = Dimension(nirrep_, "Frozen core orbitals per irrep");
    frzvpi_   = Dimension(nirrep_, "Frozen virtual orbitals per irrep");

    // Obtain memory amount from the environment
    memory_ = Process::environment.get_memory();

    nso_ = basisset_->nbf();
    nmo_ = basisset_->nbf();
    for (int k = 0; k < nirrep_; k++) {
        nsopi_[k] = dimension[k];
        nmopi_[k] = 0;
        doccpi_[k] = 0;
        soccpi_[k] = 0;
        nalphapi_[k] = 0;
        nbetapi_[k] = 0;
    }

    energy_ = 0.0;

    // Read in the debug flag
    debug_ = options_.get_int("DEBUG");

    // Read in energy convergence threshold
    energy_threshold_ = options_.get_double("E_CONVERGE");

    // Read in density convergence threshold
    density_threshold_ = options_.get_double("D_CONVERGE");;

    density_fitted_ = false;
}

double Wavefunction::compute_energy()
{
    return 0.0;
}

void Wavefunction::initialize_singletons()
{
    static bool done = false;

    if (done)
        return;

    ioff[0] = 0;
    for (size_t i=1; i<MAX_IOFF; ++i)
        ioff[i] = ioff[i-1] + i;

    df[0] = 1.0;
    df[1] = 1.0;
    df[2] = 1.0;
    for (int i=3; i<MAX_DF; ++i)
        df[i] = (i-1)*df[i-2];

    for (int i=0; i<MAX_BC; ++i)
        for (int j=0; j<=i; ++j)
            bc[i][j] = combinations(i, j);

    fac[0] = 1.0;
    for (int i=1; i<MAX_FAC; ++i)
        fac[i] = i*fac[i-1];

    done = true;
}

void Wavefunction::semicanonicalize()
{
    // Quick sanity checks
    if(Fa_ == 0 || Fb_ == 0)
        throw PSIEXCEPTION("Wavefunction: Semicanonicalize called, but Fock matrices are not initialized.");
    if(Ca_ == 0 || Cb_ == 0)
        throw PSIEXCEPTION("Wavefunction: Semicanonicalize called, but orbitals are not initialized.");
    if(Ca_ != Cb_)
        throw PSIEXCEPTION("Wavefunction: Semicanonicalize called, but orbitals are not the same.");
    if(Fa_ == Fb_)
        throw PSIEXCEPTION("Wavefunction: Semicanonicalize called, but Fock matrices are the same.");
    if(epsilon_a_ != epsilon_b_)
        throw PSIEXCEPTION("Wavefunction: Semicanonicalize called, but orbital energies are not the same.");
    // Now, make space for the new orbitals
    Cb_ = SharedMatrix(Ca_->clone());
    epsilon_b_ = SharedVector(epsilon_b_->clone());
    Ca_->set_name("Alpha semicanonical orbitals");
    Cb_->set_name("Beta semicanonical orbitals");
    epsilon_a_->set_name("Alpha semicanonical orbital energies");
    epsilon_b_->set_name("Beta semicanonical orbital energies");

    SharedMatrix Crohf = SharedMatrix(Ca_->clone());
    SharedMatrix evecs;
    SharedVector evals;

    // Transform the Fock matrix to the MO basis
    SharedMatrix moFa(Fa_->clone());
    SharedMatrix moFb(Fb_->clone());
    moFa->transform(Ca_);
    moFb->transform(Ca_);

    // Pick out occ-occ, and vir-vir subsets of the Fock matrices
    Dimension aoccpi = doccpi_ + soccpi_;
    Dimension boccpi = doccpi_;
    Dimension avirpi = nmopi_ - aoccpi;
    Dimension bvirpi = nmopi_ - boccpi;
    View aOO(moFa, aoccpi, aoccpi);
    View aVV(moFa, avirpi, avirpi, aoccpi, aoccpi);
    View bOO(moFb, boccpi, boccpi);
    View bVV(moFb, bvirpi, bvirpi, boccpi, boccpi);
    SharedMatrix aFOO = aOO();
    SharedMatrix aFVV = aVV();
    SharedMatrix bFOO = bOO();
    SharedMatrix bFVV = bVV();

    // Canonicalize the Alpha occ-occ block
    evecs = SharedMatrix(new Matrix(nirrep_, aoccpi, aoccpi));
    evals = SharedVector(new Vector(nirrep_, aoccpi));
    aFOO->diagonalize(evecs, evals);
    for(int h = 0; h < nirrep_; ++h){
        double **pC  = Crohf->pointer(h);
        double **pCa = Ca_->pointer(h);
        double **pR  = evecs->pointer(h);
        if(aoccpi[h]){
            C_DGEMM('n', 'n', nsopi_[h], aoccpi[h], aoccpi[h], 1.0, pC[0],
                    nmopi_[h], pR[0], aoccpi[h], 0.0, pCa[0], nmopi_[h]);
            for(int p = 0; p < aoccpi[h]; ++p){
                double epsilon = evals->get(h, p);
                epsilon_a_->set(h, p, epsilon);
                for(int q = 0; q < aoccpi[h]; ++q){
                    moFa->set(h, p, q, p==q ? epsilon : 0.0);
                }
            }
        }
    }
    // Canonicalize the Alpha vir-vir block
    evecs = SharedMatrix(new Matrix(nirrep_, avirpi, avirpi));
    evals = SharedVector(new Vector(nirrep_, avirpi));
    aFVV->diagonalize(evecs, evals);
    for(int h = 0; h < nirrep_; ++h){
        double **pC  = Crohf->pointer(h);
        double **pCa = Ca_->pointer(h);
        double **pR  = evecs->pointer(h);
        if(avirpi[h]){
            C_DGEMM('n', 'n', nsopi_[h], avirpi[h], avirpi[h], 1.0, &(pC[0][aoccpi[h]]),
                    nmopi_[h], pR[0], avirpi[h], 0.0, &(pCa[0][aoccpi[h]]), nmopi_[h]);
            for(int p = 0; p < avirpi[h]; ++p){
                double epsilon = evals->get(h, p);
                epsilon_a_->set(h, p+aoccpi[h], epsilon);
                for(int q = 0; q < avirpi[h]; ++q){
                    moFa->set(h, p+aoccpi[h], q+aoccpi[h], p==q ? epsilon : 0.0);
                }
            }
        }
    }
    // Canonicalize the Beta occ-occ block
    evecs = SharedMatrix(new Matrix(nirrep_, boccpi, boccpi));
    evals = SharedVector(new Vector(nirrep_, boccpi));
    bFOO->diagonalize(evecs, evals);
    for(int h = 0; h < nirrep_; ++h){
        double **pC  = Crohf->pointer(h);
        double **pCb = Cb_->pointer(h);
        double **pR  = evecs->pointer(h);
        if(boccpi[h]){
            C_DGEMM('n', 'n', nsopi_[h], boccpi[h], boccpi[h], 1.0, pC[0],
                    nmopi_[h], pR[0], boccpi[h], 0.0, pCb[0], nmopi_[h]);
            for(int p = 0; p < boccpi[h]; ++p){
                double epsilon = evals->get(h, p);
                epsilon_b_->set(h, p, epsilon);
                for(int q = 0; q < boccpi[h]; ++q){
                    moFb->set(h, p, q, p==q ? epsilon : 0.0);
                }
            }
        }
    }
    // Canonicalize the Beta vir-vir block
    evecs = SharedMatrix(new Matrix(nirrep_, bvirpi, bvirpi));
    evals = SharedVector(new Vector(nirrep_, bvirpi));
    bFVV->diagonalize(evecs, evals);
    for(int h = 0; h < nirrep_; ++h){
        double **pC  = Crohf->pointer(h);
        double **pCb = Cb_->pointer(h);
        double **pR  = evecs->pointer(h);
        if(bvirpi[h]){
            C_DGEMM('n', 'n', nsopi_[h], bvirpi[h], bvirpi[h], 1.0, &(pC[0][boccpi[h]]),
                    nmopi_[h], pR[0], bvirpi[h], 0.0, &(pCb[0][boccpi[h]]), nmopi_[h]);
            for(int p = 0; p < bvirpi[h]; ++p){
                double epsilon = evals->get(h, p);
                epsilon_b_->set(h, p+boccpi[h], epsilon);
                for(int q = 0; q < bvirpi[h]; ++q){
                    moFb->set(h, p+boccpi[h], q+boccpi[h], p==q ? epsilon : 0.0);
                }
            }
        }
    }

    /*
     * Now, we need to backtransform the Fock matrices to the SO basis
     * Ct S C = 1, so  Cinv = Ct S
     */
    SharedMatrix Cinv = SharedMatrix(new Matrix(nirrep_, nmopi_, nsopi_));
    // Alpha
    Cinv->gemm(true, false, 1.0, Ca_, S_, 0.0);
    Fa_->transform(moFa, Cinv);
    // Beta
    Cinv->gemm(true, false, 1.0, Cb_, S_, 0.0);
    Fb_->transform(moFb, Cinv);
}

boost::shared_ptr<Molecule> Wavefunction::molecule() const
{
    return molecule_;
}

boost::shared_ptr<PSIO> Wavefunction::psio() const
{
    return psio_;
}

Options& Wavefunction::options() const
{
    return options_;
}

boost::shared_ptr<IntegralFactory> Wavefunction::integral() const
{
    return integral_;
}

boost::shared_ptr<BasisSet> Wavefunction::basisset() const
{
    return basisset_;
}

boost::shared_ptr<SOBasisSet> Wavefunction::sobasisset() const
{
    return sobasisset_;
}

boost::shared_ptr<MatrixFactory> Wavefunction::matrix_factory() const
{
    return factory_;
}

boost::shared_ptr<Wavefunction> Wavefunction::reference_wavefunction() const
{
    return reference_wavefunction_;
}

void Wavefunction::set_reference_wavefunction(const boost::shared_ptr<Wavefunction> wfn)
{
    reference_wavefunction_ = wfn;
}

void Wavefunction::add_preiteration_callback(PyObject *pyobject)
{
    if (pyobject != Py_None)
        precallbacks_.push_back(pyobject);
}

void Wavefunction::add_postiteration_callback(PyObject *pyobject)
{
    if (pyobject != Py_None)
        postcallbacks_.push_back(pyobject);
}

void Wavefunction::call_preiteration_callbacks()
{
    std::vector<void*>::const_iterator iter;
    for (iter = precallbacks_.begin(); iter != precallbacks_.end(); ++iter) {
        if ((PyObject*)*iter != Py_None)
            boost::python::call<void>((PyObject*)*iter, Process::environment.reference_wavefunction());
    }
}
void Wavefunction::call_postiteration_callbacks()
{
    std::vector<void*>::const_iterator iter;
    for (iter = postcallbacks_.begin(); iter != postcallbacks_.end(); ++iter) {
        if ((PyObject*)*iter != Py_None)
            boost::python::call<void>((PyObject*)*iter, Process::environment.reference_wavefunction());
    }
}

SharedMatrix Wavefunction::Ca() const {
    if (!Ca_)
        if (!reference_wavefunction_)
            throw PSIEXCEPTION("Wavefunction::Ca: Unable to obtain MO coefficients.");
        else
            return reference_wavefunction_->Ca();

    return Ca_;
}

SharedMatrix Wavefunction::Cb() const {
    if (!Cb_)
        if (!reference_wavefunction_)
            throw PSIEXCEPTION("Wavefunction::Cb: Unable to obtain MO coefficients.");
        else
            return reference_wavefunction_->Cb();

    return Cb_;
}

std::vector<std::vector<int> > Wavefunction::subset_occupation(const Dimension& noccpi, const std::string& subset)
{
    if (!(subset == "FROZEN_OCC" || 
          subset == "FROZEN_VIR" || 
          subset == "ACTIVE_OCC" || 
          subset == "ACTIVE_VIR" || 
          subset == "FROZEN" || 
          subset == "ACTIVE" || 
          subset == "OCC" || 
          subset == "VIR" || 
          subset == "ALL"))
        throw PSIEXCEPTION("Orbital subset is not defined, should be ALL, OCC, VIR, FROZEN, ACTIVE, or look at doxygen"); 

    // Vector of relevent positions by irrep
    std::vector<std::vector<int> > positions;
    positions.resize(nirrep_);

    for (int h = 0; h < nirrep_; h++) {
        for (int i = 0; i < frzcpi_[h]; i++) {
            if (subset == "ALL" || subset == "FROZEN_OCC" || subset == "FROZEN" || subset == "OCC")
                positions[h].push_back(i);
        }
        for (int i = frzcpi_[h]; i < noccpi[h]; i++) {
            if (subset == "ALL" || subset == "ACTIVE_OCC" || subset == "ACTIVE" || subset == "OCC")
                positions[h].push_back(i);
        }
        for (int i = noccpi[h]; i < nmopi_[h] - frzvpi_[h]; i++) {
            if (subset == "ALL" || subset == "ACTIVE_VIR" || subset == "ACTIVE" || subset == "VIR")
                positions[h].push_back(i);
        }
        for (int i = nmopi_[h] - frzvpi_[h]; i < nmopi_[h]; i++) {
            if (subset == "ALL" || subset == "FROZEN_VIR" || subset == "FROZEN" || subset == "VIR")
                positions[h].push_back(i);
        }
    }
    return positions;
}
SharedMatrix Wavefunction::C_subset_helper(SharedMatrix C, const Dimension& noccpi, SharedVector epsilon, const std::string& basis, const std::string& subset)
{
    std::vector<std::vector<int> > positions = subset_occupation(noccpi, subset);

    Dimension nmopi(nirrep_);
    for (int h = 0; h < positions.size(); h++) {
        nmopi[h] = positions[h].size();
    }
    SharedMatrix C2(new Matrix("C " + basis + " " + subset, nsopi_, nmopi));
    for (int h = 0; h < positions.size(); h++) {
        for (int i = 0; i < positions[h].size(); i++) {
            C_DCOPY(nsopi_[h], &C->pointer(h)[0][positions[h][i]], nmopi_[h], &C2->pointer(h)[0][i], nmopi[h]); 
        }
    } 

    if (basis == "AO") {

        SharedMatrix C3(new Matrix("C " + basis + " " + subset, nso_, nmopi.n()));
        boost::swap(C2,C3);

        std::vector<boost::tuple<double, int, int> > order;
        for (int h = 0; h < nirrep_; h++) {
            for (int i = 0; i < positions[h].size(); i++) {
                order.push_back(boost::tuple<double,int,int>(epsilon->get(h,positions[h][i]),i,h));
            }
        } 

        std::sort(order.begin(), order.end(), std::greater<boost::tuple<double,int,int> >());
        
        for (int index = 0; index < order.size(); index++) {
            int i = boost::get<1>(order[index]);
            int h = boost::get<2>(order[index]);

            int nao = nso_;
            int nso = nsopi_[h];

            if (!nso) continue;

            C_DGEMV('N',nao,nso,1.0,AO2SO_->pointer(h)[0],nso,&C3->pointer(h)[0][i],nmopi[h],0.0,&C2->pointer()[0][index],nmopi.n());
        }

    } else if (basis == "SO" || basis == "MO") {
        // Already done
    } else {
        throw PSIEXCEPTION("Invalid basis requested, use AO, SO, or MO");
    }

    return C2;
}
SharedVector Wavefunction::epsilon_subset_helper(SharedVector epsilon, const Dimension& noccpi, const std::string& basis, const std::string& subset)
{
    std::vector<std::vector<int> > positions = subset_occupation(noccpi, subset);

    Dimension nmopi(nirrep_);
    for (int h = 0; h < positions.size(); h++) {
        nmopi[h] = positions[h].size();
    }
    
    SharedVector C2;

    if (basis == "AO") {

        C2 = SharedVector(new Vector("Epsilon " + basis + " " + subset, nmopi.n()));

        std::vector<boost::tuple<double, int, int> > order;
        for (int h = 0; h < nirrep_; h++) {
            for (int i = 0; i < positions[h].size(); i++) {
                order.push_back(boost::tuple<double,int,int>(epsilon->get(h,positions[h][i]),i,h));
            }
        } 

        std::sort(order.begin(), order.end(), std::less<boost::tuple<double,int,int> >());
        
        for (int index = 0; index < order.size(); index++) {
            C2->set(0,index,boost::get<0>(order[index]));
        }

    } else if (basis == "SO" || basis == "MO") {

        C2 = SharedVector(new Vector("Epsilon " + basis + " " + subset, nmopi));
        for (int h = 0; h < positions.size(); h++) {
            for (int i = 0; i < positions[h].size(); i++) {
                C2->set(h,i,epsilon->get(h,positions[h][i]));
            }
        } 

    } else {
        throw PSIEXCEPTION("Invalid basis requested, use AO, SO, or MO");
    }

    return C2;
}
SharedMatrix Wavefunction::D_subset_helper(SharedMatrix D, SharedMatrix C, const std::string& basis)
{
    if (basis == "AO") {
        double* temp = new double[AO2SO_->max_ncol() * AO2SO_->max_nrow()];
        SharedMatrix D2 = SharedMatrix(new Matrix("D (AO basis)", basisset_->nbf(), basisset_->nbf()));
        int symm = D->symmetry();
        for (int h = 0; h < AO2SO_->nirrep(); ++h) {
            int nao = AO2SO_->rowspi()[0];
            int nsol = AO2SO_->colspi()[h];
            int nsor = AO2SO_->colspi()[h^symm];
            if (!nsol || !nsor) continue;
            double** Ulp = AO2SO_->pointer(h);
            double** Urp = AO2SO_->pointer(h^symm);
            double** DSOp = D->pointer(h^symm);
            double** DAOp = D2->pointer();
            C_DGEMM('N','T',nsol,nao,nsor,1.0,DSOp[0],nsor,Urp[0],nsor,0.0,temp,nao);
            C_DGEMM('N','N',nao,nao,nsol,1.0,Ulp[0],nsol,temp,nao,1.0,DAOp[0],nao);
        }
        delete[] temp;
        return D2;
    } else if (basis == "SO") {
        return SharedMatrix(D->clone());
    } else if (basis == "MO") {
        SharedMatrix D2(new Matrix("D (MO Basis)", C->colspi(), C->colspi()));

        int symm = D->symmetry();
        int nirrep = D->nirrep();

        double* SC = new double[C->max_ncol() * C->max_nrow()];
        double* temp = new double[C->max_ncol() * C->max_nrow()];
        for (int h = 0; h < nirrep; h++) {
            int nmol = C->colspi()[h];
            int nmor = C->colspi()[h^symm];
            int nsol = C->rowspi()[h];
            int nsor = C->rowspi()[h^symm];
            if (!nmol || !nmor || !nsol || !nsor) continue;
            double** Slp = S_->pointer(h);
            double** Srp = S_->pointer(h^symm);
            double** Clp = C->pointer(h);
            double** Crp = C->pointer(h^symm);
            double** Dmop = D2->pointer(h);
            double** Dsop = D->pointer(h);

            C_DGEMM('N','N',nsor,nmor,nsor,1.0,Srp[0],nsor,Crp[0],nmor,0.0,SC,nmor);
            C_DGEMM('N','N',nsol,nmor,nsor,1.0,Dsop[0],nsor,SC,nmor,0.0,temp,nmor);
            C_DGEMM('N','N',nsol,nmol,nsol,1.0,Slp[0],nsol,Clp[0],nmol,0.0,SC,nmol);
            C_DGEMM('T','N',nmol,nmor,nsol,1.0,SC,nmol,temp,nmor,0.0,Dmop[0],nmor);
        }
        delete[] temp;
        delete[] SC;
        return D;
    } else {
        throw PSIEXCEPTION("Invalid basis requested, use AO, SO, or MO");
    }
}

SharedMatrix Wavefunction::Ca_subset(const std::string& basis, const std::string& subset)
{
    return C_subset_helper(Ca_, nalphapi_, epsilon_a_, basis, subset);
}
SharedMatrix Wavefunction::Cb_subset(const std::string& basis, const std::string& subset)
{
    return C_subset_helper(Cb_, nbetapi_, epsilon_b_, basis, subset);
}
SharedMatrix Wavefunction::Da_subset(const std::string& basis)
{
    return D_subset_helper(Da_, Ca_, basis);
}
SharedMatrix Wavefunction::Db_subset(const std::string& basis)
{
    return D_subset_helper(Db_, Cb_, basis);
}
SharedVector Wavefunction::epsilon_a_subset(const std::string& basis, const std::string& subset)
{
    return epsilon_subset_helper(epsilon_a_, nalphapi_, basis, subset);
}
SharedVector Wavefunction::epsilon_b_subset(const std::string& basis, const std::string& subset)
{
    return epsilon_subset_helper(epsilon_b_, nbetapi_, basis, subset);
}

SharedMatrix Wavefunction::Fa() const
{
    return Fa_;
}

SharedMatrix Wavefunction::Fb() const
{
    return Fb_;
}

boost::shared_ptr<Matrix> Wavefunction::Lagrangian() const
{
    return Lagrangian_;
}

boost::shared_ptr<Matrix> Wavefunction::tpdm_gradient_contribution() const
{
    throw PSIEXCEPTION("This type of wavefunction has not defined a TPDM gradient contribution!");
}

boost::shared_ptr<Vector> Wavefunction::epsilon_a() const
{
    return epsilon_a_;
}

boost::shared_ptr<Vector> Wavefunction::epsilon_b() const
{
    return epsilon_b_;
}

const SharedMatrix Wavefunction::Da() const
{
    return Da_;
}

SharedMatrix Wavefunction::Db() const
{
    return Db_;
}

SharedMatrix Wavefunction::X() const
{
    return Lagrangian_;
}

SharedMatrix Wavefunction::gradient() const
{
    return gradient_;
}

void Wavefunction::set_gradient(SharedMatrix& grad)
{
    gradient_ = grad;
}

boost::shared_ptr<Vector> Wavefunction::frequencies() const
{
    return frequencies_;
}

void Wavefunction::set_frequencies(boost::shared_ptr<Vector>& freqs)
{
    frequencies_ = freqs;
}

void Wavefunction::save() const
{
}
