#ifndef TTTSelector_h
#define TTTSelector_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TH1.h>
#include <TH2.h>
#include <TEfficiency.h>
#include <exception>
#include <iostream>

// Headers needed by this particular selector
#include <vector>
#include "Analysis/VVAnalysis/interface/ScaleFactor.h"
#include "Analysis/VVAnalysis/interface/SelectorBase.h"
#include "Analysis/VVAnalysis/interface/BranchManager.h"
#include "Analysis/VVAnalysis/interface/GoodParticle.h"

class TTTSelector : public SelectorBase {
 public :
  ScaleFactor* pileupSF_;
  ScaleFactor* muonSF_;
  ScaleFactor* eIdSF_ ;
  ScaleFactor* eGsfSF_;
  ScaleFactor* mIdSF_;
  ScaleFactor* mIsoSF_;

  // Derived variables
  bool isVBS_;
  bool passesLeptonVeto;
  Float_t weight;

  // Common variables
  Float_t genWeight;
  Float_t ZMass;
  Float_t Mass;
  Float_t MET;
  Float_t type1_pfMETPhi;

  //NanoAOD variables
  static const unsigned int N_KEEP_MU_E_ = 10;
  static const unsigned int N_KEEP_JET_ = 35;
  UInt_t nElectron;
  UInt_t nMuon;
  Float_t Electron_pt[N_KEEP_MU_E_];
  Float_t Electron_eta[N_KEEP_MU_E_];
  Float_t Electron_phi[N_KEEP_MU_E_];
  Float_t Electron_mass[N_KEEP_MU_E_];
  Float_t Muon_pt[N_KEEP_MU_E_];
  Float_t Muon_eta[N_KEEP_MU_E_];
  Float_t Muon_phi[N_KEEP_MU_E_];
  Float_t Muon_mass[N_KEEP_MU_E_];
  Int_t Electron_cutBased[N_KEEP_MU_E_];
  Int_t Electron_charge[N_KEEP_MU_E_];
  Int_t Muon_charge[N_KEEP_MU_E_];
  Bool_t Muon_tightId[N_KEEP_MU_E_];
  Bool_t Muon_mediumId[N_KEEP_MU_E_];
  UChar_t Muon_tkIsoId[N_KEEP_MU_E_];
  Float_t Muon_pfRelIso04_all[N_KEEP_MU_E_];
  Int_t numPU;

  UInt_t nJet;
  Float_t Jet_btagCSVV2[N_KEEP_JET_];
  Float_t Jet_btagDeepB[N_KEEP_JET_];
  Float_t Jet_eta[N_KEEP_JET_];
  Float_t Jet_phi[N_KEEP_JET_];
  Float_t Jet_pt[N_KEEP_JET_];
  Float_t Jet_mass[N_KEEP_JET_];
  
  /* UInt_t nWZTightMuon; */
  /* UInt_t nWZMediumMuon; */
  /* Bool_t l1IsTight; */
  /* Bool_t l2IsTight; */
  /* Bool_t l3IsTight; */
  /* Float_t l1GenPt; */
  /* Float_t l2GenPt; */
  /* Float_t l3GenPt; */
  /* Bool_t e1IsEB; */
  /* Bool_t e2IsEB; */
  /* Bool_t e3IsEB; */
  /* Float_t e1PVDXY; */
  /* Float_t e2PVDXY; */
  /* Float_t e3PVDXY; */
  /* Float_t e1PVDZ; */
  /* Float_t e2PVDZ; */
  /* Float_t e3PVDZ; */
  /* Float_t m1RelPFIsoDBR04; */
  /* Float_t m2RelPFIsoDBR04; */
  /* Float_t m3RelPFIsoDBR04; */
  /* Float_t l1Eta; */
  /* Float_t l2Eta; */
  /* Float_t l3Eta; */
  /* Float_t l1Pt; */
  /* Float_t l2Pt; */
  /* Float_t l3Pt; */
  /* Float_t l3MtToMET; */
  /* Float_t Flag_BadChargedCandidateFilterPass; */
  /* Float_t Flag_BadPFMuonFilterPass; */
  /* Bool_t Flag_duplicateMuonsPass; */
  /* Bool_t Flag_badMuonsPass; */
  /* Bool_t Flag_HBHENoiseFilterPass; */
  /* Bool_t Flag_HBHENoiseIsoFilterPass; */
  /* Bool_t Flag_EcalDeadCellTriggerPrimitiveFilterPass; */
  /* Bool_t Flag_goodVerticesPass; */
  /* Bool_t Flag_eeBadScFilterPass; */
  /* Bool_t Flag_globalTightHalo2016FilterPass; */
    
  std::vector<GoodPart> goodParts;
  
  
  // Readers to access the data (delete the ones you do not need).
  //virtual void    SetScaleFactors() override;
  virtual void    SlaveBegin(TTree *tree) override;
  virtual void    Init(TTree *tree) override;

  ClassDefOverride(TTTSelector,0);

 protected:
  std::vector<std::string> nonprompt3l_ = {
    "tt-lep", "st-schan", "st-tchan-t", "st-tchan-tbar",
    "st-tw", "st-tbarw", "DYm50", "DYm50-1j",
    "DYm50-2j","DYm50-3j","DYm50-4j", "DYm50__LO",
  };

  bool isNonpromptEstimate_;
  bool isNonpromptMC_;
  bool isZgamma_;
  const float FR_MAX_PT_ = 50;
  const float FR_MAX_ETA_ = 2.5;
  bool IsTightMuon(size_t);
  bool IsTightElectron(size_t);
  bool IsTightJet(size_t);
  bool IsTightBJet(size_t);
  bool IsOverlap(size_t);

  int getSRBin();
  double HT;
  int nTightJet, nBJets;
  
  virtual std::string GetNameFromFile() override;
    virtual void    SetBranchesNanoAOD() override;
    virtual void    SetBranchesUWVV() override;
    void LoadBranchesUWVV(Long64_t entry, std::pair<Systematic, std::string> variation) override;
    void LoadBranchesNanoAOD(Long64_t entry, std::pair<Systematic, std::string> variation) override;
    void ApplyScaleFactors();
    void SetShiftedMasses();
    void FillHistograms(Long64_t entry, std::pair<Systematic, std::string> variation) override;
    virtual void    SetupNewDirectory() override;

    BranchManager b;
  
  };

#endif
