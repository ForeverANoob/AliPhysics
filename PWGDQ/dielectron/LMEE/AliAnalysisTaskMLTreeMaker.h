#ifndef ALIANALYSISTASKMLTREEMAKER_H
#define ALIANALYSISTASKMLTREEMAKER_H
class TH1F;
class TList;
class TH2D;
class TH3D;
//class AliESDtrackCuts;


#include "AliAnalysisTaskSE.h"
#include "AliDielectronVarCuts.h"
#include "AliDielectronTrackCuts.h"
#include "AliDielectronCutGroup.h"
#include "AliDielectronPID.h"
#include "AliAnalysisFilter.h"
#include "AliDielectronEventCuts.h"
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#ifndef ALIANALYSISTASKSE_H
#endif

// Authors: Sebastian Lehner (SMI Vienna) - selehner@cern.ch


class AliAnalysisTaskMLTreeMaker : public AliAnalysisTaskSE {
 public:
  AliAnalysisTaskMLTreeMaker(const char *name, TString TMVAWeightFileName);
  AliAnalysisTaskMLTreeMaker();
  ~AliAnalysisTaskMLTreeMaker(); 
  

  AliDielectronEventCuts* eventCuts;
  AliDielectronVarCuts *eventplaneCuts;
  AliAnalysisFilter* evfilter;
  
  AliDielectronVarCuts* trcuts;
  AliDielectronTrackCuts *trfilter;
  AliDielectronPID *pidcuts;
  AliDielectronCutGroup* cuts;
  AliAnalysisFilter* filter; 
  

  // need this to use PID in dielectron framework
  AliDielectronVarManager* varManager;
     
  virtual void   UserCreateOutputObjects();
  virtual void   UserExec(Option_t *option);
  virtual void   FinishTaskOutput();
  virtual void   Terminate(Option_t *);
  int CheckGenerator(int Index);
  
  void SetCorrWidthMean(Int_t det, TH3D* width, TH3D* mean){
    if(det==1){  fmeanITS=mean; fwidthITS=width;}
    if(det==2){  fmeanTPC=mean; fwidthTPC=width;}
    if(det==3){  fmeanTOF=mean; fwidthTOF=width;}   
  };

  void SetupTrackCuts(AliDielectronCutGroup* f);
  void SetupEventCuts(AliDielectronEventCuts* f);
  void SetupTMVAReader(TString TMVAweight);
  
  void isMC(Bool_t isMC){hasMC=isMC;}
  
  void SetCentralityPercentileRange(Double_t min, Double_t max){
    fCentralityPercentileMin = min;
    fCentralityPercentileMax = max;

    Printf("Thresholds Set");
    Printf("pT = %f - %f",fPtMin,fPtMax);
    Printf("eta = %f - %f",fEtaMin,fEtaMax);
    Printf("cent = %f - %f",fCentralityPercentileMin,fCentralityPercentileMax);
  }

  void SetPtRange(Double_t min, Double_t max){
    fPtMin = min;
    fPtMax = max;

    Printf("Thresholds Set");
    Printf("pT = %f - %f",fPtMin,fPtMax);
    Printf("eta = %f - %f",fEtaMin,fEtaMax);
    Printf("cent = %f - %f",fCentralityPercentileMin,fCentralityPercentileMax);
  }

  void SetEtaRange(Double_t min, Double_t max){
    fEtaMin = min;
    fEtaMax = max;

    Printf("Thresholds Set");
    Printf("pT = %f - %f",fPtMin,fPtMax);
    Printf("eta = %f - %f",fEtaMin,fEtaMax);
    Printf("cent = %f - %f",fCentralityPercentileMin,fCentralityPercentileMax);
  }

  void SetFilterBit(Int_t filterBit){
    fFilterBit = filterBit;
  }
  
    void SetLoCuts(Bool_t x){       //use loose cuts?
    loCuts = x;
  }

  void SetESigRangeITS(Double_t min, Double_t max){
    fESigITSMin = min;
    fESigITSMax = max;
    Printf("ITS electron nSigma values (inclusive)");
    Printf(" Min = %f, Max = %f", fESigITSMin, fESigITSMax);
  }

  void SetESigRangeTPC(Double_t min, Double_t max){
    fESigTPCMin = min;
    fESigTPCMax = max;
    Printf("TPC electron nSigma values (inclusive)");
    Printf(" Min = %f, Max = %f", fESigTPCMin, fESigTPCMax);
  }

  void SetESigRangeTOF(Double_t min, Double_t max){
    fESigTOFMin = min;
    fESigTOFMax = max;
    Printf("TOF electron nSigma values (inclusive)");
    Printf(" Min = %f, Max = %f", fESigTOFMin, fESigTOFMax);
  }

  
  void SetPSigRangeTPC(Double_t min, Double_t max){
    fPSigTPCMin = min;
    fPSigTPCMax = max;
    Printf("TPC pion nSigma values (exclusive)");
    Printf(" Min = %f, Max = %f", fPSigTPCMin, fPSigTPCMax);
  }
  
  void TPCPionPID( Bool_t answer = kTRUE){ 
    fUsePionPIDTPC = answer;
    Printf("No pion PID in TPC applied");
  }

  void StorePionSigmaValues( Bool_t answer = kFALSE){
    fPionSigmas = answer;
    Printf("Pion nSigma values for ITS, TPC and TOF will be written to tree");
  }

  void StoreKaonSigmaValues( Bool_t answer = kFALSE){
    fKaonSigmas = answer;
    Printf("Kaon nSigma values for ITS, TPC and TOF will be written to tree");
  }
  
  void SetUseCorr( Bool_t b){
      fuseCorr=b;
  }
  
  void SetUseTMVA(Bool_t b){
      useTMVA=b;
  }
 private:
     
  TMVA::Reader* TMVAReader;     //-> 
  Bool_t useTMVA;
  AliPIDResponse *fPIDResponse;     //! PID response object

  //AliPIDCombined *fPIDCombined;    
     
  std::vector<Double_t> eta;
  std::vector<Double_t> phi;
  std::vector<Double_t> pt;
  std::vector<Int_t> charge;

//  std::vector<Int_t> NClustersITS;
  std::vector<Float_t> NCrossedRowsTPC;
  std::vector<Int_t> NClustersTPC;
  std::vector<Bool_t> HasSPDfirstHit; 
  std::vector<Double_t> RatioCrossedRowsFindableClusters;  
  std::vector<Int_t> NTPCSignal; 
  std::vector<unsigned int> fGeneratorHashs; // will be filled with the TString hashes of the generators you want to be analyzed
  Bool_t loCuts;        //loose cuts?
  
//  std::vector<Int_t> IsBG;
  
  Int_t runn;
  Int_t n;
  Double_t cent;
  Double_t ZDCepA;
  Double_t ZDCepC;
  Double_t TPCep;
  Double_t TPCepA;
  Double_t TPCepC;
  Double_t NTPCclsEv;
  
  TList* fQnList;
  AliAnalysisManager *man;

  
  Double_t IsEventAccepted(AliVEvent *event);
  Int_t GetAcceptedTracks(AliVEvent *event, Double_t gCentrality);
  Bool_t GetDCA(const AliVEvent* event, const AliAODTrack *track, Double_t* d0z0, Double_t* covd0z0);
  Bool_t FillZDCEventPlane(Double_t* ZDCevArr);
  void FillQnEventplanes(TList* qnlist);
  AliAnalysisTaskMLTreeMaker(const AliAnalysisTaskMLTreeMaker&); // not implemented

  AliAnalysisTaskMLTreeMaker& operator=(const AliAnalysisTaskMLTreeMaker&); // not implemented


  TList *fList;//output list for QA histograms

  Double_t fCentralityPercentileMin;// minimum centrality threshold (default = 0)
  Double_t fCentralityPercentileMax;// maximum centrality threshold (default = 80)

  Double_t fPtMin;// minimum pT threshold (default = 0)
  Double_t fPtMax;// maximum pT threshold (default = 1000)
  Double_t fEtaMin;// minimum eta threshold (default = -10)
  Double_t fEtaMax;// maximum eta threshold (default = 10)

  //Electron nSigma values, initilaised to common cut values but can be set manually 
  Double_t fESigITSMin;
  Double_t fESigITSMax;
  Double_t fESigTPCMin;
  Double_t fESigTPCMax;
  Double_t fESigTOFMin;
  Double_t fESigTOFMax;
  //Pion nSigma values, initilaised to common cut values but can be set manually 
  Double_t fPSigTPCMin; 
  Double_t fPSigTPCMax; 

  Bool_t fUsePionPIDTPC; //Use Pion nSigma information in TPC

  //Flags to write extra particle PID information
  //Initiliased to kFALSE
  Bool_t fPionSigmas; 
  Bool_t fKaonSigmas;

  Int_t fFilterBit;// track cut bit from track selection (default = kTPCqualSPDany)

//  AliESDtrackCuts* fESDTrackCuts;
  
  Int_t gMultiplicity;
  Int_t mcTrackIndex;
  AliMCEvent* fMcArray;
  AliMCEvent* mcEvent; 

  std::vector<Double_t> EsigTPC;
  std::vector<Double_t> EsigTOF;
  std::vector<Double_t> EsigITS;
  
  std::vector<Double_t> PsigTPC;
  std::vector<Double_t> PsigTOF;
  std::vector<Double_t> PsigITS;

  std::vector<Double_t> KsigTPC;
  std::vector<Double_t> KsigTOF;
  std::vector<Double_t> KsigITS;

  Bool_t hasMC;
  Bool_t Rej;
 
  std::vector<Double_t> MCpt;
  std::vector<Double_t> MCeta;
  std::vector<Double_t> MCphi;
  
  std::vector<Double_t> MCvertx;
  std::vector<Double_t> MCverty;
  std::vector<Double_t> MCvertz;
  
  
  std::vector<Int_t> glabel ;
  std::vector<Int_t> gLabelFirstMother ;
  std::vector<Int_t> gLabelMinFirstMother ;
  std::vector<Int_t> gLabelMaxFirstMother ;
  std::vector<Int_t> iGenIndex ;
  std::vector<Int_t> iPdgFirstMother ;
  
  std::vector<Float_t> dcar;    //DCA
  std::vector<Float_t> dcaz;
  
  Double_t vertx;
  Double_t verty;
  Double_t vertz; 
  
  std::vector<Int_t> nITS;
  std::vector<Double_t> nITSshared;
  
  std::vector<Int_t> ITS1S;
  std::vector<Int_t> ITS2S;
  std::vector<Int_t> ITS3S;
  std::vector<Int_t> ITS4S;
  std::vector<Int_t> ITS5S;
  std::vector<Int_t> ITS6S;
  
  std::vector<Double_t> chi2ITS;
//  std::vector<Double_t> chi2TPC;
  std::vector<Double_t> chi2GlobalPerNDF;
  std::vector<Double_t> chi2GlobalvsTPC;
  Int_t	fCutMaxChi2TPCConstrainedVsGlobalVertexType;
  
  std::vector<Int_t> pdg;
  std::vector<Int_t> pdgmother;
  std::vector<Int_t> hasmother;
  std::vector<Int_t> label;
  std::vector<Int_t> motherlabel;
  
  Float_t nITSTMVA;
  Float_t ITS1SharedTMVA;
  Float_t ITS2SharedTMVA;
  Float_t ITS3SharedTMVA;
  Float_t ITS4SharedTMVA;
  Float_t ITS5SharedTMVA;
  Float_t ITS6SharedTMVA;
  Float_t nITSshared_fracTMVA;
  Float_t NCrossedRowsTPCTMVA;
  Float_t NClustersTPCTMVA;
  Float_t NTPCSignalTMVA;
  Float_t logDCAxyTMVA ;
  Float_t logDCAzTMVA ;   
  Float_t chi2GlobalPerNDFTMVA;
  Float_t chi2ITSTMVA;
  Float_t etaTMVA;
  Float_t phiTMVA;
  Float_t ptTMVA;   
  Float_t centTMVA;
  
  std::vector<Float_t> MVAout;
  
  TString TMVAWeightFileName;
  
  TH3D* fwidthTPC;
  TH3D* fmeanTPC;
  TH3D* fwidthITS;
  TH3D* fmeanITS;
  TH3D* fwidthTOF;
  TH3D* fmeanTOF;
  
  Bool_t fuseCorr;
  Bool_t fIsTMVAInit;
  //TBits*            fUsedVars;                // used variables by AliDielectronVarManager
  
//  Double_t probs[AliPID::kSPECIESC];

  TTree* fTree;
  TH1F* fQAHist;
  
//  TH2D* fHistTrackStats;//QA histogram for track filter bit statistics vs. centrality

//  TH3D* fHistEtaPhiPt;//QA histogram for eta/phi/pt distribution


  ClassDef(AliAnalysisTaskMLTreeMaker, 1); 

};



#endif
