#include "Analysis/VVAnalysis/interface/LowPileupWSelector.h"
#include <TStyle.h>
#include <regex>

void LowPileupWSelector::Init(TTree *tree)
{
    //doSystematics_ = false;
    doSystematics_ = true;
    allChannels_ = {{mp, "mp"}, {mn, "mn"}};
    hists1D_ = {"CutFlow", "mW", "mtW", "mtWUncorr", "yW", "ptW", "ptl", "etal", "pfMet",};
    systHists_ = {"ptW", "ptl", "mtW", "pfMet"};
    systematics_ = {
        {muonEfficiencyMCSubtractUp, "CMS_eff_MCsubt_mUp"},
        {muonEfficiencyMCSubtractDown, "CMS_eff_MCsubt_mDown"},
        {modelingFsrUp, "CMS_modeling_fsrUp"},
        {modelingFsrDown, "CMS_modeling_fsrDown"},
        {muonEfficiencyBackgroundUp, "CMS_eff_background_mUp"},
        {muonEfficiencyBackgroundDown, "CMS_eff_background_mDown"},
        {muonEfficiencyTagPtUp, "CMS_eff_tagPt_mUp"},
        {muonEfficiencyTagPtDown, "CMS_eff_tagPt_mDown"},
        {muonEfficiencyStatUp, "CMS_eff_stat_mUp"},
        {muonEfficiencyStatDown, "CMS_eff_stat_mDown"},
    };
    
    //enum{mc=1,fsr,bkg,tagpt,effstat,pfireu,pfired};
    systematicWeightMap_ = {
        {Central, 0},
        {muonEfficiencyMCSubtractUp, 1},
        {muonEfficiencyMCSubtractDown, 1},
        {modelingFsrUp, 2},
        {modelingFsrDown, 2},
        {muonEfficiencyBackgroundUp, 3},
        {muonEfficiencyBackgroundDown, 3},
        {muonEfficiencyTagPtUp, 4},
        {muonEfficiencyTagPtDown, 4},
        {muonEfficiencyStatUp, 5},
        {muonEfficiencyStatDown, 5},
    };

    fReader.SetTree(tree);
    LowPileupSelector::Init(tree);
}

void LowPileupWSelector::SetBranchesBacon() {
    LowPileupSelector::SetBranchesBacon();
}

void LowPileupWSelector::LoadBranchesBacon(Long64_t entry, SystPair variation) { 
    if (variation.first == Central) {
        fReader.SetLocalEntry(entry);
        LowPileupSelector::LoadBranchesBacon(entry, variation);
        if (*charge > 0) {
            channel_ = mp;
            channelName_ = "mp";
        }
        else {
            channel_ = mn;
            channelName_ = "mn";
        }
    }
    if (isMC_) {
        float cenwgt = evtWeight[systematicWeightMap_[Central]];
        float wgt = evtWeight[systematicWeightMap_[variation.first]];
        if (variation.first == Central)
            weight = cenwgt;
        else if (variation.first == muonEfficiencyMCSubtractUp ||
                    variation.first == muonEfficiencyBackgroundUp ||
                    variation.first == muonEfficiencyTagPtUp ||
                    variation.first == muonEfficiencyStatUp ||
                    variation.first == modelingFsrUp) {
            weight = wgt > cenwgt ? wgt : cenwgt + std::abs(cenwgt-wgt);
        }
        else {
            weight = wgt < cenwgt ? wgt : cenwgt - std::abs(cenwgt-wgt);
        }
    }
    int metIndex = (isMC_ && (isW_ || isZ_) && !isNonprompt_);
    pfMet = metVector.At(metIndex);
    pfMetPhi = metPhiVector.At(metIndex);
}

void LowPileupWSelector::SetComposite() {
    pfMetVec = TLorentzVector();
    pfMetVec.SetPtEtaPhiM(pfMet, 0., pfMetPhi, 0.);
    wCand = *lep + pfMetVec;
    //mtW = std::sqrt(2*lep->Pt()*pfMet*(1-std::cos(lep->Phi()-pfMetVec.Phi())));
}

void LowPileupWSelector::FillHistograms(Long64_t entry, SystPair variation) { 
    if (lep->Pt() < 25)
        return;
    SafeHistFill(histMap1D_, "mW", channel_, variation.first, wCand.M(), weight);
    SafeHistFill(histMap1D_, "mtW", channel_, variation.first, *mtW, weight);
    SafeHistFill(histMap1D_, "mtWUncorr", channel_, variation.first, *mtWuncorr, weight);
    SafeHistFill(histMap1D_, "ptW", channel_, variation.first, wCand.Pt(), weight);
    SafeHistFill(histMap1D_, "yW", channel_, variation.first, wCand.Rapidity(), weight);
    SafeHistFill(histMap1D_, "ptl", channel_, variation.first, lep->Pt(), weight);
    SafeHistFill(histMap1D_, "etal", channel_, variation.first, lep->Eta(), weight);
    SafeHistFill(histMap1D_, "pfMet", channel_, variation.first, pfMet, weight);

    if (subprocessHistMaps1D_.empty())
        return;

    //std::vector<int> binning = {0, 13, 26, 38, 50, 62, 75, 100};
    std::vector<int> binning = {0, 5, 10, 15, 20, 25, 30, 40, 50, 60, 75, 100};
    size_t upperIndex = std::distance(binning.begin(), std::upper_bound(binning.begin(), binning.end(), genVPt));

    std::string binname = name_;
    binname.append("_GenPtW_");
    if (upperIndex == binning.size())
        binname.append(std::to_string(binning.back()));
    else {
        binname.append(std::to_string(binning.at(upperIndex-1)));
        binname.append("_");
        binname.append(std::to_string(binning.at(upperIndex)));
    }

    try {
        HistMap1D& subprocessMap = subprocessHistMaps1D_.at(binname);
        SafeHistFill(subprocessMap, "mW", channel_, variation.first, wCand.M(), weight);
        SafeHistFill(subprocessMap, "ptW", channel_, variation.first, wCand.Pt(), weight);
        SafeHistFill(subprocessMap, "yW", channel_, variation.first, wCand.Rapidity(), weight);
        SafeHistFill(subprocessMap, "ptl", channel_, variation.first, lep->Pt(), weight);
        SafeHistFill(subprocessMap, "etal", channel_, variation.first, lep->Eta(), weight);
        SafeHistFill(subprocessMap, "pfMet", channel_, variation.first, pfMet, weight);
    }
    catch (std::exception& e) {
        throw std::range_error("Could not find bin " + binname + " in subprocessMap!");
    }
}