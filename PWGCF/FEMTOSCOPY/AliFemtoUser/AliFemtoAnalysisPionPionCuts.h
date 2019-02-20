///
/// \file AliFemtoAnalysisPionPionCuts.h
///

#pragma once

#ifndef ALIFEMTOANALYSISPIONPIONCUTS_H
#define ALIFEMTOANALYSISPIONPIONCUTS_H

#include "AliFemtoEventCut.h"
#include "AliFemtoTrackCut.h"
#include "AliFemtoPairCut.h"

#include "AliFemtoCutAttrEvent.h"
#include "AliFemtoCutAttrTrack.h"
#include "AliFemtoCutAttrPairTrack.h"

#include <TList.h>


namespace pwgfemto {

// event-cut
typedef AddEventCutAttrs<
          EventCutAttrCentrality,
          AddEventCutAttrs<
            EventCutAttrVertexZ,
            EventCutAttrTrigger
          >
        > EventCutAttrsAK;

// track-cut
typedef TrackSelectionCut<
          // non-physics properties
          AddTrackCutAttrs<
            TrackCutAttrStatus,
            AddTrackCutAttrs<
              TrackCutAttrMinNclsTPC,
              AddTrackCutAttrs<
               TrackCutAttrMinNclsITS,
               TrackCutAttrCharge> > >,


          // physics-cuts
          AddTrackCutAttrs<
              TrackCutAttrImpact,
                AddTrackCutAttrs<
                  TrackCutAttrPt,
                  AddTrackCutAttrs<
	                TrackCutAttrEta,
                    AddTrackCutAttrs<
	                  TrackCutAttrChi2TPC,
                      TrackCutAttrChi2ITS> > > >

        > TrackCutAttrsAK;

// paircut
typedef AddPairCutAttrs<
          PairCutTrackAttrSameLabel,
            AddPairCutAttrs<
              PairCutTrackAttrPt,
              PairCutTrackAttrShareQuality> > PairCutAttrsBaseAK;

typedef AddPairCutAttrs<
          PairCutAttrsBaseAK,
          PairCutTrackAttrAvgSep> PairCutAttrsAvgSepAK;

typedef AddPairCutAttrs<
          PairCutAttrsBaseAK,
          PairCutTrackAttrDetaDphi> PairCutAttrsDphiDetaAK;

}  // namespace pwgfemto


/// \class AliFemtoEventCutPionPionAK
/// \brief Andrew Kubera's event cut for identical-pion analysis
///
///
class AliFemtoEventCutPionPionAK : public AliFemtoEventCutAttr<AliFemtoEventCutPionPionAK, pwgfemto::EventCutAttrsAK> {
public:

  typedef AliFemtoEventCutAttr<AliFemtoEventCutPionPionAK, pwgfemto::EventCutAttrsAK> Super;
  typedef pwgfemto::EventCutAttrsAK CutAttrs;

  AliFemtoEventCutPionPionAK()
    : Super()
    {}

  AliFemtoEventCutPionPionAK(AliFemtoConfigObject &cfg)
    : Super(cfg)
    {}

  virtual ~AliFemtoEventCutPionPionAK()
    {}

  virtual void AppendSettings(TCollection &) const;

  static const char* ClassName()
    { return "AliFemtoEventCutPionPionAK"; }
};


/// \class AliFemtoTrackCutPionPionAK
/// \brief Andrew Kubera's pion-track cut
///
class AliFemtoTrackCutPionPionAK : public AliFemtoTrackCutAttr<AliFemtoTrackCutPionPionAK, pwgfemto::TrackCutAttrsAK> {
public:
  typedef AliFemtoTrackCutAttr<AliFemtoTrackCutPionPionAK, pwgfemto::TrackCutAttrsAK> Super;
  typedef pwgfemto::TrackCutAttrsAK CutAttrs;

  AliFemtoTrackCutPionPionAK()
    : fNumPass(0)
    , fNumFail(0)
    {}

  AliFemtoTrackCutPionPionAK(AliFemtoConfigObject &cfg)
    : Super(cfg)
    , fNumPass(0)
    , fNumFail(0)
    {}

  virtual ~AliFemtoTrackCutPionPionAK()
    { }

  virtual void AppendSettings(TCollection &, TString prefix="") const;

  virtual bool Pass(const AliFemtoTrack *track)
    {
      bool passes = CutAttrs::PassSelection(*track);
      if (passes) {
        passes = CutAttrs::PassCut(*track);
        ++(passes ? fNumPass : fNumFail);
      }
      return passes;
    }

  static const char* ClassName()
    { return "AliFemtoTrackCutPionPionAK"; }

  ULong_t fNumPass,
          fNumFail;
};


/// \class AliFemtoPairCutPionPionAKAvgSep
/// \brief Andrew Kubera's average separation pair cut
///
class AliFemtoPairCutPionPionAKAvgSep : public AliFemtoPairCutAttrTracks<AliFemtoPairCutPionPionAKAvgSep,
                                                                         pwgfemto::PairCutAttrsAvgSepAK> {
public:

  typedef AliFemtoPairCutAttrTracks<AliFemtoPairCutPionPionAKAvgSep, pwgfemto::PairCutAttrsAvgSepAK> Super;

  virtual ~AliFemtoPairCutPionPionAKAvgSep();

  AliFemtoPairCutPionPionAKAvgSep();
  AliFemtoPairCutPionPionAKAvgSep(AliFemtoConfigObject &cfg)
    : Super(cfg)
    {}

  /// user-written method to return string describing cuts
  virtual AliFemtoString Report();

  virtual void AppendSettings(TCollection &) const;

  static const char* ClassName()
    { return "AliFemtoPairCutPionPionAKAvgSep"; }
};


/// \class AliFemtoPairCutPionPionAKDetaDphi
/// \brief Andrew Kubera's Deta-Dphi pair cut
///
class AliFemtoPairCutPionPionAKDetaDphi : public AliFemtoPairCutAttrTracks<AliFemtoPairCutPionPionAKDetaDphi,
                                                                           pwgfemto::PairCutAttrsDphiDetaAK> {
public:

  typedef AliFemtoPairCutAttrTracks<AliFemtoPairCutPionPionAKDetaDphi, pwgfemto::PairCutAttrsDphiDetaAK> Super;

  virtual ~AliFemtoPairCutPionPionAKDetaDphi()
    { }

  AliFemtoPairCutPionPionAKDetaDphi()
    : Super()
    { }

  AliFemtoPairCutPionPionAKDetaDphi(AliFemtoConfigObject &cfg)
    : Super(cfg)
    { }

  virtual void EventBegin(const AliFemtoEvent *ev);

  /// user-written method to return string describing cuts
  virtual AliFemtoString Report();

  virtual void AppendSettings(TCollection &) const;

  static const char* ClassName()
    { return "AliFemtoPairCutPionPionAKDetaDphi"; }
};



template <>
AliFemtoConfigObject AliFemtoConfigObject::From(const AliFemtoEventCutPionPionAK &cut);

template <>
AliFemtoConfigObject AliFemtoConfigObject::From(const AliFemtoTrackCutPionPionAK &cut);

template <>
AliFemtoConfigObject AliFemtoConfigObject::From(const AliFemtoPairCutPionPionAKAvgSep &cut);

template <>
AliFemtoConfigObject AliFemtoConfigObject::From(const AliFemtoPairCutPionPionAKDetaDphi &cut);

template <>
AliFemtoEventCutPionPionAK* AliFemtoConfigObject::Into<AliFemtoEventCutPionPionAK>(bool);

template <>
AliFemtoTrackCutPionPionAK* AliFemtoConfigObject::Into<AliFemtoTrackCutPionPionAK>(bool);

template <>
AliFemtoPairCutPionPionAKAvgSep* AliFemtoConfigObject::Into<AliFemtoPairCutPionPionAKAvgSep>(bool);

template <>
AliFemtoPairCutPionPionAKDetaDphi* AliFemtoConfigObject::Into<AliFemtoPairCutPionPionAKDetaDphi>(bool);



#endif