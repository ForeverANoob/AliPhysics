/// \file AddTaskCaloTrackCorrBase.C
/// \ingroup CaloTrackCorrMacros
/// \brief Configuration AliAnalysisTaskCaloTrackCorrelation base functionalities
///
/// Configuration macro AliAnalysisTaskCaloTrackCorrelation base functionalities.
/// AliCaloTrackReader and AliCalorimeterUtils functionalities initialized and activated.
/// Another configuration macro should be attached to run on analysis, 
/// like AddTaskMultipleTrackCutIsoConeAnalysis.C that needs also ConfigureCaloTrackCorrAnalysis.C
///
/// \author Gustavo Conesa Balbastre <Gustavo.Conesa.Balbastre@cern.ch>, (LPSC-CNRS)

// Set includes for compilation

#if !defined(__CINT__) || defined(__MAKECINT__)

// ROOT
#include <TString.h>
#include <TSystem.h>
#include <TROOT.h>

// AliPhysics
#include "AliAnalysisManager.h"
#include "AliInputEventHandler.h"
#include "AliVTrack.h"
#include "AliAODTrack.h"
#include "AliESDtrack.h"
#include "AliESDtrackCuts.h"

// CaloTrackCorrelations frame
#include "AliCaloTrackESDReader.h"
#include "AliCaloTrackAODReader.h"
#include "AliCalorimeterUtils.h"
#include "AliAnalysisTaskCaloTrackCorrelation.h"
#include "AliAnaCaloTrackCorrMaker.h"

// Macros
R__ADD_INCLUDE_PATH($ALICE_PHYSICS)
#include "PWGGA/CaloTrackCorrelations/macros/ConfigureAndGetEventTriggerMaskAndCaloTriggerString.C"
#include "PWGJE/macros/CreateTrackCutsPWGJE.C"
#include "PWGGA/CaloTrackCorrelations/macros/CheckActiveEMCalTriggerPerPeriod.C"

#endif

///
/// Configure the EMCal/DCal cluster filtering cuts done in AliCaloTrackReader
///
/// \param reader: pointer to AliCaloTrackReaderTask
/// \param clustersArray : A string with the array of clusters not being the default (default is empty string)
/// \param col: A string with the colliding system
/// \param year: The year the data was taken, used to configure time cut
/// \param simulation : A bool identifying the data as simulation
/// \param rejectEMCTrig : An int to reject EMCal triggered events with bad trigger: 0 no rejection, 1 old runs L1 bit, 2 newer runs L1 bit
/// \param trigger :  A string with the trigger class, abbreviated, defined in ConfigureAndGetEventTriggerMaskAndCaloTriggerString.
/// \param minCen : An int to select the minimum centrality, -1 means no selection
/// \param maxCen : An int to select the maximum centrality, -1 means no selection
///
void ConfigureEventSelection( AliCaloTrackReader * reader, TString cutsString, 
                             TString col          , Int_t  year,  Bool_t  simulation, 
                             Bool_t  rejectEMCTrig, TString trigger,
                             Int_t   minCen       , Int_t  maxCen                )
{
  //
  // Vertex criteria
  //
  reader->SetZvertexCut(10.);               
  reader->SwitchOnPrimaryVertexSelection(); // and besides primary vertex is found
  reader->SwitchOffRejectNoTrackEvents();   // careful if on and productions with no TPC (muon_calo) 
 
  reader->SwitchOffRecalculateVertexBC();
  reader->SwitchOffVertexBCEventSelection();
  
  reader->SwitchOffV0ANDSelection() ;       // and besides v0 AND
  //reader->RejectFastClusterEvents() ;

  //
  // Pile-up
  //
  if ( cutsString.Contains("SPDPileUp") )
  {
    printf("AddTaskCaloTrackCorrBase::ConfigureReader() - Switch on Pile-up event rejection by SPD\n");
    reader->SwitchOnPileUpEventRejection();  // remove pileup by default off, apply it only for MB not for trigger
    if ( year > 2013 ) reader->SetPileUpParamForSPD(0,5);
  }
  
  //
  // Centrality
  //
  if ( col == "PbPb" )
  {
    reader->SetCentralityClass("V0M");
    reader->SetCentralityOpt(100);  // 10 (c= 0-10, 10-20 ...), 20  (c= 0-5, 5-10 ...) or 100 (c= 1, 2, 3 ..)
    reader->SetCentralityBin(minCen,maxCen); // Accept all events, if not select range
    
    //reader->SwitchOnAcceptOnlyHIJINGLabels();
    
    // Event plane (only used in Maker and mixing for AliAnaPi0/AliAnaHadronCorrelation for the moment)
    reader->SetEventPlaneMethod("V0");
  }
  
  //
  // In case of Pythia pt Hard bin simulations (jet-jet, gamma-jet)
  // reject some special events that bother the cross section
  //
  if ( simulation && cutsString.Contains("PtHardCut"))
  {
    // Event rejection cuts for jet-jet simulations, do not use in other
    if (  cutsString.Contains("JetJet")  )
    {
      reader->SetPtHardAndJetPtComparison(kTRUE);
      reader->SetPtHardAndJetPtFactor(2);
    }
    
    // Event rejection more suitable for gamma-jet simulations, do not use in other
    if (  cutsString.Contains("GamJet")  )
    {    
      reader->SetPtHardAndClusterPtComparison(kTRUE);
      reader->SetPtHardAndClusterPtFactor(1.5);
    }
    
    // Set here generator name, default pythia
    //reader->GetMCAnalysisUtils()->SetMCGenerator("");
  }
  
  //
  // Calorimeter Trigger Selection
  //
  
  //if(!simulation) reader->SetFiredTriggerClassName("CEMC7EGA-B-NOPF-CENTNOTRD"); // L1 Gamma
  
  // Event triggered by EMCal selection settings
  // very old ways, not up to date
  reader->SwitchOffTriggerPatchMatching();
  reader->SwitchOffBadTriggerEventsRemoval();
  
  if ( rejectEMCTrig > 0 && !simulation && (trigger.Contains("EMC") || trigger.Contains("L")) )
  {
    printf("AddTaskCaloTrackCorrBase::ConfigureReader() === Remove bad triggers === \n");
    reader->SwitchOnTriggerPatchMatching();
    reader->SwitchOnBadTriggerEventsRemoval();
    
    //    reader->SetTriggerPatchTimeWindow(8,9); // default values
    //    if     (kRunNumber < 146861) reader->SetEventTriggerL0Threshold(3.);
    //    else if(kRunNumber < 154000) reader->SetEventTriggerL0Threshold(4.);
    //    else if(kRunNumber < 165000) reader->SetEventTriggerL0Threshold(5.5);
    //    //redefine for other periods, triggers
    //
    //    if(kRunNumber < 172000)
    //    {
    //      reader->SetEventTriggerL1Bit(4,5); // current LHC11 data
    //      printf("\t Old L1 Trigger data format!\n");
    //    }
    //    else
    //    {
    //      reader->SetEventTriggerL1Bit(6,8); // LHC12-13 data
    //      printf("\t Current L1 Trigger data format!\n");
    //    }
    
    //reader->SwitchOffTriggerClusterTimeRecal() ;
  } 
}

///
/// Configure the EMCal/DCal cluster filtering cuts done in AliCaloTrackReader
///
/// \param reader: pointer to AliCaloTrackReaderTask
/// \param calorimeter : A string with he calorimeter used to measure the trigger particle: EMCAL, DCAL, PHOS
/// \param cutsString : A string with additional cuts ("Smearing","MCEnScale")
/// \param clustersArray : A string with the array of clusters not being the default (default is empty string)
/// \param year: The year the data was taken, used to configure time cut and fiducial cut
/// \param simulation : A bool identifying the data as simulation
///
void ConfigureEMCALClusterCuts ( AliCaloTrackReader* reader, 
                                 TString calorimeter, TString cutsString, TString clustersArray,
                                 Int_t year, Bool_t simulation )
{
  reader->SetEMCALEMin(0.3);
  reader->SetEMCALEMax(1000);
  
  if      ( calorimeter == "EMCAL" )
  {
     if     ( year > 2014 ) reader->GetFiducialCut()->SetSimpleEMCALFiducialCut(0.67,  81.2, 185.8) ; //12 SM
    else if ( year > 2010 ) reader->GetFiducialCut()->SetSimpleEMCALFiducialCut(0.67,  81.2, 178.8) ; //10 SM
    else                    reader->GetFiducialCut()->SetSimpleEMCALFiducialCut(0.67,  81.2, 118.8) ; // 4 SM
  }
  else if ( calorimeter == "DCAL"  ) reader->GetFiducialCut()->SetSimpleEMCALFiducialCut(0.67, 261.2, 325.8) ; 
  
  // Use other cluster array than default:
  reader->SetEMCALClusterListName(clustersArray);
  
  // Time cuts
  reader->SwitchOffUseParametrizedTimeCut();
  reader->SwitchOffUseEMCALTimeCut();
  reader->SetEMCALTimeCut(-1e10,1e10); // Open time cut
  
  // For data, check what is the range needed depending on the sample
  if ( !simulation ) 
  {
    printf("AddTaskCaloTrackCorrBase::ConfigureReader() - Apply time cut:");
    reader->SwitchOnUseEMCALTimeCut();
    reader->SetEMCALTimeCut(-30,30); // rather open, until high pT calibration is improved
    printf(" -30 ns < t < 30 ns\n");
  }
  
  if ( calorimeter == "EMCAL" || calorimeter == "DCAL" )
  {
    reader->SwitchOnEMCALCells();
    reader->SwitchOnEMCAL();
  }
  
  // EMCal shower shape smearing
  // Set it in the train configuration page not here for the moment
  if ( simulation && cutsString.Contains("Smearing"))
  {
    reader->SwitchOnShowerShapeSmearing(); // Active only on MC, off by default
    
    reader->SetSmearingFunction(AliCaloTrackReader::kSmearingLandau);
    reader->SetShowerShapeSmearWidth(0.005);
    
    //reader->SetSmearingFunction(AliCaloTrackReader::kSmearingLandauShift);
    //reader->SetShowerShapeSmearWidth(0.035);
  }
  
  // Energy scale factor for 100 MeV clusterization threshold in MC
  // and TRD vs no TRD SMs
  // Factors reported in https://alice-notes.web.cern.ch/node/837
  if ( cutsString.Contains("MCEnScale") && simulation )
  {
    printf("AddTaskCaloTrackCorrBase::ConfigureEMCALClusterCuts() - Set global energy scale!\n");
    reader->SwitchOnClusterEScalePerSMCorrection() ;
    
    if     ( year == 2011 )
    {
      for(Int_t ism = 0; ism < 6; ism++) 
        reader->SetScaleFactorPerSM(ism, 1./1.012); 
      for(Int_t ism = 6; ism < 10; ism++) 
        reader->SetScaleFactorPerSM(ism, 1./0.998);
    }
    else if( year == 2012 )
    {
      for(Int_t ism = 0; ism < 4; ism++) 
        reader->SetScaleFactorPerSM(ism, 1./1.001); 
      for(Int_t ism = 4; ism < 10; ism++) 
        reader->SetScaleFactorPerSM(ism, 1./0.995);
    }
    else if( year == 2013 ) // Needs to be revised
    {
      for(Int_t ism = 0; ism < 4; ism++) 
        reader->SetScaleFactorPerSM(ism, 1./0.997); 
      for(Int_t ism = 4; ism < 10; ism++) 
        reader->SetScaleFactorPerSM(ism, 1./0.993);
    }
    else
      printf("AddTaskCaloTrackCorrBase::ConfigureEMCALClusterCuts() - Global scale not defined for such case\n");
  }
}

///
/// Configure the PHOS cluster filtering cuts done in AliCaloTrackReader
///
/// \param reader: pointer to AliCaloTrackReaderTask
/// \param calorimeter : A string with he calorimeter used to measure the trigger particle: EMCAL, DCAL, PHOS
/// \param year: The year the data was taken, used to configure fiducial cut
///
void ConfigurePHOSClusterCuts ( AliCaloTrackReader* reader, 
                                TString calorimeter, Int_t year )
{
  reader->SetPHOSEMin(0.3);
  reader->SetPHOSEMax(1000);
  
  if ( year > 2014 ) reader->GetFiducialCut()->SetSimplePHOSFiducialCut (0.125, 250.5, 319.5) ; 
  else               reader->GetFiducialCut()->SetSimplePHOSFiducialCut (0.125, 260.5, 319.5) ; 
  
  if ( calorimeter == "PHOS" )
  { // Should be on if QA is activated with correlation on
    reader->SwitchOnPHOSCells();
    reader->SwitchOnPHOS();
  }
}

///
/// Configure the track filtering cuts done in AliCaloTrackReader
///
/// \param reader: pointer to AliCaloTrackReaderTask
/// \param inputDataType: string with data Type "ESD", "AOD"
/// \param cutsString : A string with additional cuts ("ITSonly")
///
void ConfigureTrackCuts ( AliCaloTrackReader* reader, 
                          TString inputDataType, TString cutsString)
{
  reader->SwitchOnCTS();

  reader->SetCTSPtMin(0.2);
  reader->SetCTSPtMax(1000);
  
  reader->GetFiducialCut()->SetSimpleCTSFiducialCut(0.9, 0, 360) ;
  
  reader->SwitchOffUseTrackTimeCut();
  reader->SetTrackTimeCut(0,50);
  
  reader->SwitchOffUseTrackDCACut();
  //reader->SetTrackDCACut(0,0.0105);
  //reader->SetTrackDCACut(1,0.035);
  //reader->SetTrackDCACut(2,1.1);
  
  if ( inputDataType == "ESD" )
  {
#if defined(__CINT__)
    gROOT->LoadMacro("$ALICE_PHYSICS/PWGJE/macros/CreateTrackCutsPWGJE.C");
#endif

    //AliESDtrackCuts * esdTrackCuts = CreateTrackCutsPWGJE(10041004);
    //reader->SetTrackCuts(esdTrackCuts);
    
    if ( cutsString.Contains("ITSonly") )
    {
      reader->SetTrackStatus(AliESDtrack::kITSpureSA);
    }
    // Hybrid TPC+ITS
    else
    {
      AliESDtrackCuts * esdTrackCuts  = CreateTrackCutsPWGJE(10001008);
      reader->SetTrackCuts(esdTrackCuts);
      AliESDtrackCuts * esdTrackCuts2 = CreateTrackCutsPWGJE(10011008);
      reader->SetTrackComplementaryCuts(esdTrackCuts2);
      
      reader->SwitchOnConstrainTrackToVertex();
    }
  }
  else if ( inputDataType == "AOD" )
  {
    if ( cutsString.Contains("ITSonly") )
    {
      reader->SetTrackStatus(AliAODTrack::kTrkITSsa);
    }
    else
    {
      reader->SwitchOnAODHybridTrackSelection(); // Check that the AODs have Hybrids!!!!
      reader->SwitchOnAODTrackSharedClusterSelection();
      reader->SetTrackStatus(AliVTrack::kITSrefit);
    }
    
    //reader->SwitchOnAODPrimaryTrackSelection(); // Used in preliminary results of QM from Nicolas and Xiangrong?
    //reader->SwitchOnTrackHitSPDSelection();     // Check that the track has at least a hit on the SPD, not much sense to use for hybrid or TPC only tracks
    //reader->SetTrackFilterMask(128);            // Filter bit, not mask, use if off hybrid, TPC only
  }
}

///
/// Configure the class handling the events and cluster/tracks filtering.
/// each item is configured in dedicated methods.
///
/// \param col: A string with the colliding system
/// \param simulation : A bool identifying the data as simulation
/// \param clustersArray : A string with the array of clusters not being the default (default is empty string)
/// \param calorimeter : A string with he calorimeter used to measure the trigger particle: EMCAL, DCAL, PHOS
/// \param cutsString : A string with additional cuts (Smearing, SPDPileUp)
/// \param trigger :  A string with the trigger class, abbreviated, defined in ConfigureAndGetEventTriggerMaskAndCaloTriggerString.C
/// \param nonLinOn : A bool to set the use of the non linearity correction
/// \param calibrate : Use own calibration tools, do not rely on EMCal correction framewor or clusterizer
/// \param year: The year the data was taken, used to configure some histograms and cuts
/// \param rejectEMCTrig : An int to reject EMCal triggered events with bad trigger: 0 no rejection, 1 old runs L1 bit, 2 newer runs L1 bit
/// \param minCen : An int to select the minimum centrality, -1 means no selection
/// \param maxCen : An int to select the maximum centrality, -1 means no selection
/// \param printSettings : A bool to enable the print of the settings per task
/// \param debug : An int to define the debug level of all the tasks
///
AliCaloTrackReader * ConfigureReader(TString col,           Bool_t simulation,
                                     TString clustersArray, TString calorimeter, 
                                     TString cutsString,    
                                     Bool_t  nonLinOn,      Bool_t calibrate,
                                     Int_t   year,          
                                     TString trigger,       Bool_t rejectEMCTrig,
                                     Int_t   minCen,        Int_t  maxCen,
                                     Bool_t  printSettings, Int_t  debug         )
{
  // Get the data type ESD or AOD
  AliAnalysisManager * mgr = AliAnalysisManager::GetAnalysisManager();
  TString inputDataType = mgr->GetInputEventHandler()->GetDataType();
  
  AliCaloTrackReader * reader = 0;
  if     (inputDataType == "AOD") reader = new AliCaloTrackAODReader();
  else if(inputDataType == "ESD") reader = new AliCaloTrackESDReader();
  else printf("AddTaskCaloTrackCorrBase::ConfigureReader() - Data not known InputData=%s\n",inputDataType.Data());
  
  reader->SetDebug(debug);//10 for lots of messages

  //------------------------  
  // Event selection cuts
  //------------------------
  ConfigureEventSelection(reader, cutsString, col, year, simulation, 
                          rejectEMCTrig, trigger, minCen, maxCen);
  
  //------------------------
  // Detector input filtering
  //------------------------
  
  // Fiducial cuts active, see acceptance in detector methods
  reader->SwitchOnFiducialCut();
  
  ConfigureTrackCuts       (reader, inputDataType, cutsString);
  
  ConfigurePHOSClusterCuts (reader, calorimeter, year);

  ConfigureEMCALClusterCuts(reader, calorimeter, cutsString, clustersArray, year, simulation);

  // Extra calorimeter stuff:
  //
  // In case no external calibrated cluster/cell list or EMCal correction framework applied before
  if ( calibrate ) reader->SwitchOnClusterRecalculation();
  else             reader->SwitchOffClusterRecalculation();
  
  // CAREFUL, make sure not done previously in the tender/clusterizer/EM correction framework
  if ( nonLinOn ) reader->SwitchOnClusterELinearityCorrection();
  else            reader->SwitchOffClusterELinearityCorrection();
  
  if(printSettings) reader->Print("");
  
  return reader;
}

///
/// Configure the class handling the calorimeter clusters specific methods
///
/// \param col: A string with the colliding system
/// \param simulation : A bool identifying the data as simulation
/// \param calorimeter : A string with he calorimeter used to measure the trigger particle: EMCAL, DCAL, PHOS
/// \param nonLinOn : A bool to set the use of the non linearity correction
/// \param calibrate : Use own calibration tools, do not rely on EMCal correction framewor or clusterizer
/// \param year: The year the data was taken, used to configure some histograms
/// \param printSettings : A bool to enable the print of the settings per task
/// \param debug : An int to define the debug level of all the tasks
///
AliCalorimeterUtils* ConfigureCaloUtils(TString col,         Bool_t simulation, 
                                        TString calorimeter, Bool_t nonLinOn,      
                                        Bool_t calibrate,    Int_t   year,        
                                        Bool_t printSettings, Int_t   debug)
{
  AliCalorimeterUtils *cu = new AliCalorimeterUtils;
  
  cu->SetDebug(debug);
  
  // Remove clusters close to borders, at least max energy cell is 1 cell away
  cu->SetNumberOfCellsFromEMCALBorder(0); // temporary! set it back to 1.
  cu->SetNumberOfCellsFromPHOSBorder (2);
  
  cu->SetNumberOfSuperModulesUsed(10);
  
  if ( calorimeter == "PHOS" )
  {
    if(year < 2014) cu->SetNumberOfSuperModulesUsed(3);
    else            cu->SetNumberOfSuperModulesUsed(4);
  }
  else 
  {
    Int_t nSM     = 20;
    Int_t lastEMC = 11;
    if      (year == 2010) { nSM =  4; lastEMC = 3; }// EMCAL first year
    else if (year <  2014) { nSM = 10; lastEMC = 9; }// EMCAL active 2011-2013
    
    cu->SetNumberOfSuperModulesUsed(nSM);
    
    if      (calorimeter.Contains("EMCAL"))
    {
      cu->SetFirstSuperModuleUsed( 0);
      cu->SetLastSuperModuleUsed (lastEMC);
    }
    else if (calorimeter.Contains("DCAL"))
    {
      cu->SetFirstSuperModuleUsed(12);
      cu->SetLastSuperModuleUsed (19);
    }
    else
    {
      cu->SetFirstSuperModuleUsed(0);
      cu->SetLastSuperModuleUsed (cu->GetNumberOfSuperModulesUsed()-1);
    }
    
    printf("AddTaskCaloTrackCorrBase::CalorimeterUtils() - nSM %d, first %d, last %d\n",
           cu->GetNumberOfSuperModulesUsed(),cu->GetFirstSuperModuleUsed(), cu->GetLastSuperModuleUsed());
  }
  
  // Search of local maxima in cluster
  if ( col == "pp" )
  {
    cu->SetLocalMaximaCutE(0.1);
    cu->SetLocalMaximaCutEDiff(0.03);
  }
  else
  {
    cu->SetLocalMaximaCutE(0.2);
    cu->SetLocalMaximaCutEDiff(0.03);
  }
  
  cu->SwitchOffRecalculateClusterTrackMatching();
  
  cu->SwitchOnBadChannelsRemoval() ;
  
  // EMCAL settings
  
  if ( !simulation )
    cu->SwitchOnLoadOwnEMCALGeometryMatrices();
  
  AliEMCALRecoUtils * recou = cu->GetEMCALRecoUtils();
  
  // Calibrations, do nothing by default
  Bool_t calibEner = kFALSE;
  Bool_t calibTime = kFALSE;
  cu->SwitchOffRecalibration();
  cu->SwitchOffRunDepCorrection();
  
  if ( calibrate )
  {
    cu->SwitchOnRecalibration(); 
    cu->SwitchOnRunDepCorrection();
    
    calibEner = kTRUE;
    calibTime = kTRUE;
  }
  
  cu->ConfigureEMCALRecoUtils(simulation,
                              kTRUE,      // exotic
                              nonLinOn,   // Non linearity
                              calibEner,  // E calib
                              kTRUE,      // bad map
                              calibTime); // time calib
  
  //if( calibTime ) recou->SetExoticCellDiffTimeCut(50);
  
  //if( nonLinOn )  cu->SwitchOnCorrectClusterLinearity(); // Done in Configure method
  
  if ( printSettings )
  {
    printf("AddTaskCaloTrackCorrBase::ConfigureCaloUtils() - EMCAL Recalibration ON? %d %d\n",
           recou->IsRecalibrationOn(), cu->IsRecalibrationOn());
    printf("AddTaskCaloTrackCorrBase::ConfigureCaloUtils() - EMCAL BadMap        ON? %d %d\n",
           recou->IsBadChannelsRemovalSwitchedOn(), cu->IsBadChannelsRemovalSwitchedOn());
  }
  
  // PHOS
  cu->SwitchOffLoadOwnPHOSGeometryMatrices();
  
  if(printSettings) cu->Print("");
  
  return cu;
}


///
/// Main method calling all the configuration
/// Creates a CaloTrackCorr task, configures it and adds it to the analysis manager.
///
/// The options that can be passed to the macro are:
/// \param calorimeter : A string with he calorimeter used to measure the trigger particle: EMCAL, DCAL, PHOS
/// \param simulation : A bool identifying the data as simulation
/// \param year: The year the data was taken, used to configure some histograms
/// \param col: A string with the colliding system
/// \param period: A string with the data period
/// \param rejectEMCTrig : An int to reject EMCal triggered events with bad trigger: 0 no rejection, 1 old runs L1 bit, 2 newer runs L1 bit
/// \param clustersArray : A string with the array of clusters not being the default (default is empty string)
/// \param cutsString : A string with additional cuts (Smearing, SPDPileUp)
/// \param nonLinOn : A bool to set the use of the non linearity correction
/// \param minCen : An int to select the minimum centrality, -1 means no selection
/// \param maxCen : An int to select the maximum centrality, -1 means no selection
/// \param mixOn : A bool to switch the correlation mixing analysis
/// \param calibrate : Use own calibration tools, do not rely on EMCal correction framewor or clusterizer
/// \param outputfile : A string to change the name of the histograms output file, default is AnalysisResults.root
/// \param printSettings : A bool to enable the print of the settings per task
/// \param debug : An int to define the debug level of all the tasks
/// \param trigSuffix :  A string with the trigger class, abbreviated, defined in ConfigureAndGetEventTriggerMaskAndCaloTriggerString.C
///
/// Options of cutsString:
///    * Smearing: Smear shower shape long axis in cluster, just this parameter. Not to be used
///    * SPDPileUp: Remove events tagged as pile-up by SPD
///    * MCWeight: Apply weight on histograms and calculate sumw2
///    * MCEvtHeadW: Get the cross section from the event header
///    * MCEnScale: Scale the cluster energy by a factor, depending SM number  and period
///    * ITSonly: Select tracks with only ITS information
///    * PtHardCut: Select events with jet or cluster photon energy not too large or small with respect the generated partonic energy 
///       * JetJet: Compare generated (reconstructed generator level) jet pT with parton pT  
///       * GamJet: Compare cluster pt and generated parton pt, careful, test before using
///
AliAnalysisTaskCaloTrackCorrelation * AddTaskCaloTrackCorrBase
(
 TString  calorimeter   = "EMCAL", // "DCAL", "PHOS"
 Bool_t   simulation    = kFALSE,
 Int_t    year          = -1, // 2011,
 TString  col           = "", // pp
 TString  period        = "", // LHC11d
 Int_t    rejectEMCTrig = 0,
 TString  clustersArray = "",
 TString  cutsString    = "", 
 Bool_t   calibrate     = kFALSE,
 Bool_t   nonLinOn      = kFALSE,
 Int_t    minCen        = -1,
 Int_t    maxCen        = -1,
 Bool_t   mixOn         = kTRUE,
 TString  outputfile    = "",
 Bool_t   printSettings = kFALSE,
 Int_t    debug         = 0,  
 const char *trigSuffix = "EMC7"
)
{
  // Check the global variables, and reset the provided ones if empty.
  //
  TString trigger = trigSuffix;
  
  // Get the pointer to the existing analysis manager via the static access method.
  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  if (!mgr)
  {
    ::Error("AddTask", "No analysis manager to connect to.");
    return NULL;
  }
  
  // Check the analysis type using the event handlers connected to the analysis manager.
  
  if (!mgr->GetInputEventHandler())
  {
    ::Error("AddTask", "This task requires an input event handler");
    return NULL;
  }
  
  // Name for containers
  
  TString anaCaloTrackCorrBase = Form("CTC_%s_Trig_%s",calorimeter.Data(),trigger.Data());

  if ( col=="PbPb" && 
       maxCen>=0      )     anaCaloTrackCorrBase+=Form("_Cen%d_%d",minCen,maxCen);
  if ( clustersArray!="" )  anaCaloTrackCorrBase+=Form("_Cl%s",clustersArray.Data());
  if ( mixOn          )     anaCaloTrackCorrBase+="_MixOn";
  if ( cutsString!="" )     anaCaloTrackCorrBase+="_"+cutsString;

  printf("AddTaskCaloTrackCorrBase::Main() <<<< Folder name: %s >>>>>\n",
         anaCaloTrackCorrBase.Data());
      
  //
  // Create task, pass the maker and add it to the manager
  //
  AliAnalysisTaskCaloTrackCorrelation * task = new AliAnalysisTaskCaloTrackCorrelation (Form("%s",anaCaloTrackCorrBase.Data()));
  
  //  task->SetFirstEvent(1800);
  //  task->SetLastEvent (2000);  
  
  task->SetDebugLevel(debug);
  
  //task->SetBranches("ESD:AliESDRun.,AliESDHeader");
  //task->SetConfigFileName(""); //Don't configure the analysis via configuration file.
  
  AliAnaCaloTrackCorrMaker * maker = new AliAnaCaloTrackCorrMaker();
  
  maker->SetAnaDebug(debug)  ;
  
  task->SetAnalysisMaker(maker);
  
  mgr->AddTask(task);

  //
  // Create containers
  //
  if ( outputfile.Length() == 0 )
    outputfile = AliAnalysisManager::GetCommonFileName();
  
  AliAnalysisDataContainer *cout_pc   = mgr->CreateContainer(anaCaloTrackCorrBase, TList::Class(),
                                                             AliAnalysisManager::kOutputContainer,
                                                             Form("%s",outputfile.Data()));
  
  AliAnalysisDataContainer *cout_cuts = mgr->CreateContainer(Form("Param_%s",anaCaloTrackCorrBase.Data()), TList::Class(),
                                                             AliAnalysisManager::kParamContainer,
                                                             "AnalysisParameters.root");
  
  // Create ONLY the output containers for the data produced by the task.
  // Get and connect other common input/output containers via the manager as below
  //==============================================================================
  mgr->ConnectInput  (task, 0, mgr->GetCommonInputContainer());
  //if(!kData.Contains("delta")   && outputAOD) mgr->ConnectOutput (task, 0, mgr->GetCommonOutputContainer());
  mgr->ConnectOutput (task, 1, cout_pc);
  mgr->ConnectOutput (task, 2, cout_cuts);
  //==============================================================================
  
  // Do not configure the wagon for certain analysis combinations
  // But create the task so that the sub-wagon train can run
  //
#if defined(__CINT__)
  gROOT->LoadMacro("$ALICE_PHYSICS/PWGGA/CaloTrackCorrelations/macros/CheckActiveEMCalTriggerPerPeriod.C");
#endif

  Bool_t doAnalysis = CheckActiveEMCalTriggerPerPeriod(simulation,trigger,period,year);
  
  if ( doAnalysis && calorimeter == "DCAL" && year < 2015 ) doAnalysis = kFALSE;
  
  if ( !doAnalysis ) 
  {
    maker->SwitchOffProcessEvent();
    return task;
  }
  
  // #### Start analysis configuration ####
  // Print settings
  //
  printf("AddTaskCaloTrackCorrBase::Main() << settings:"
         "\n calorimeter <%s>, simulation <%d>, year <%d>, col <%s>, "
         "\n trigger <%s>, reject EMC <%d>, clustersArray <%s>, cuts <%s>"
         "\n calibrate <%d>, non linearity <%d>, minCen <%d>, maxCen <%d>, "
         "\n mixOn <%d>, outputfile <%s>, printSettings <%d>, debug <%d> >>\n",
         calorimeter.Data(), simulation, year, col.Data(),
         trigger.Data(), rejectEMCTrig, clustersArray.Data(), cutsString.Data(),
         calibrate, nonLinOn, minCen, maxCen,
         mixOn, outputfile.Data(), printSettings, debug);
  
  // Make sure the B field is enabled for track selection, some cuts need it
  ((AliInputEventHandler*)mgr->GetInputEventHandler())->SetNeedField(kTRUE);
  
  //
  // General frame setting and configuration
  //
  maker->SetReader   ( ConfigureReader   (col,simulation,clustersArray,calorimeter,cutsString,
                                          nonLinOn,calibrate,year,trigger,rejectEMCTrig,
                                          minCen,maxCen,printSettings,debug) );  
  
  maker->SetCaloUtils( ConfigureCaloUtils(col,simulation,calorimeter,nonLinOn,calibrate,year,
                                          printSettings,debug) );
  
  maker->SwitchOnHistogramsMaker()  ;
  maker->SwitchOnAODsMaker()  ;
  
  if ( simulation || !trigger.Contains("EMC") ) 
    maker->SwitchOffDataControlHistograms();
  
  if ( simulation )
  {
    // Calculate the cross section weights, apply them to all histograms 
    // and fill xsec and trial histo. Sumw2 must be activated.
    if ( cutsString.Contains("MCWeight") )
    {
      maker->GetReader()->GetWeightUtils()->SwitchOnMCCrossSectionCalculation(); 
      maker->SwitchOnSumw2Histograms();
    }
    else
    {
      // Just fill cross section and trials histograms.
      maker->GetReader()->GetWeightUtils()->SwitchOnMCCrossSectionHistoFill();
    }
    
    // For recent productions where the cross sections and trials are not stored in separate file
    if ( cutsString.Contains("MCEvtHeadW") )
      maker->GetReader()->GetWeightUtils()->SwitchOnMCCrossSectionFromEventHeader() ;
    
    // For productions where the cross sections and trials are not stored in separate file
    TString prodType = gSystem->Getenv("ALIEN_JDL_LPMPRODUCTIONTYPE");
    printf("AddTaskCaloTrackCorrBase() - MC production name: %s\n",prodType.Data());
    if ( prodType.Contains("LHC16c") ) // add here any other affected periods, for the moment jet-jet 8 TeV
    {   
      printf("\t use the cross section from EventHeader per Event\n");
      maker->GetReader()->GetWeightUtils()->SwitchOnMCCrossSectionFromEventHeader() ;
    } 
    
    // Add control histogram with pT hard to control aplication of weights 
    maker->SwitchOnPtHardHistogram();
    
    // Apply particle pT weights
    //maker->SwitchOnMCParticlePtWeights();
  }
  
  if ( printSettings ) maker->Print("");
  
  // Set the list name for later recovery in macros 
  maker->GetListOfAnalysisContainers()->SetName(anaCaloTrackCorrBase);
  
  // Select events trigger depending on trigger
  //
  maker->GetReader()->SwitchOnEventTriggerAtSE(); // on is default case
  if ( !simulation )
  {
#if defined(__CINT__)
    gROOT->LoadMacro("$ALICE_PHYSICS/PWGGA/CaloTrackCorrelations/macros/ConfigureAndGetEventTriggerMaskAndCaloTriggerString.C");
#endif

    TString caloTriggerString = "";
    UInt_t mask = ConfigureAndGetEventTriggerMaskAndCaloTriggerString(trigger, year, caloTriggerString);

    maker->GetReader()->SetFiredTriggerClassName(caloTriggerString);

    // For mixing with AliAnaParticleHadronCorrelation switch it off
    if ( mixOn )
    {
      maker->GetReader()->SwitchOffEventTriggerAtSE();
      maker->GetReader()->SetEventTriggerMask(mask); 
      // what to do with caloTriggerString?
      
      // Careful, not all productions work with kMB, try kINT7, kINT1, kAnyINT
      //reader->SetMixEventTriggerMask(AliVEvent::kMB); 
      maker->GetReader()->SetMixEventTriggerMask(AliVEvent::kINT7); 
      
      printf("AddTaskCaloTrackCorrBase::Main() << Trigger selection done in AliCaloTrackReader!!! >>> \n");
    }
    else
    {
      task ->SelectCollisionCandidates( mask );
    }
  }
  
  printf("AddTaskCaloTrackCorrBase::Main() << End Base Task Configuration for %s >>\n",
         anaCaloTrackCorrBase.Data());

  return task;
}


