# Cmssw-UserCode for EECs <!-- omit from toc -->

This package provides CMSSW software for running EEC analysis. This includes:

 - Event selection utilities for Z->mumu
 - Jet preselection utilities 
 - Support for calculation and writing to NanoAOD projected, resolved 3-point, and resolved 4-point EECs
 - Support for (highly-configurable) matching of tracks between GEN and RECO
 - Computation of EEC detector effects from the gen matching 
 - Useful NanoAOD tables 

Details on all of this are below

##  Table of contents <!-- omit from toc -->

- [1. setup instructions](#1-setup-instructions)
- [2. Event selection](#2-event-selection)
- [3. Jet building and preselection](#3-jet-building-and-preselection)
  - [3.1 CMSSW products](#31-cmssw-products)
  - [3.2 NanoAOD products](#32-nanoaod-products)
- [4. SimonJets](#4-simonjets)
  - [4.1 Systematic variations](#41-systematic-variations)
  - [4.2 SimonJets from CMSSW jets](#42-simonjets-from-cmssw-jets)
  - [4.3 NANO tables](#43-nano-tables)
- [5. Gen-matching](#5-gen-matching)
  - [5.1 Delta-R controls](#51-delta-r-controls)
  - [5.2 Resolutions](#52-resolutions)
  - [5.3 Charge matching](#53-charge-matching)
  - [5.4 Flavor matching](#54-flavor-matching)
  - [5.5 NANO tables](#55-nano-tables)
- [6. EEC calculations](#6-eec-calculations)
  - [6.1 Types of EEC observables:](#61-types-of-eec-observables)
    - [6.1.1 Projected EECs (referred to throughout the code as "proj")](#611-projected-eecs-referred-to-throughout-the-code-as-proj)
    - [6.1.2 Resolved three-point EECs (referred to throughout the code as "res3")](#612-resolved-three-point-eecs-referred-to-throughout-the-code-as-res3)
    - [6.1.3 Resolved four-point EECs (referred to throughout the code as "res4")](#613-resolved-four-point-eecs-referred-to-throughout-the-code-as-res4)
  - [6.2 EEC calculator data formats ("resultType")](#62-eec-calculator-data-formats-resulttype)
    - [6.2.1 Unbinned EECs (resultType="Unbinned")](#621-unbinned-eecs-resulttypeunbinned)
    - [6.2.2 Prebinned, unmerged EECs (resultType="Vector")](#622-prebinned-unmerged-eecs-resulttypevector)
    - [6.2.3 Prebinned, merged EECs (resultType="Array")](#623-prebinned-merged-eecs-resulttypearray)
  - [6.3 EEC denominators](#63-eec-denominators)
  - [6.4 Jet preselections](#64-jet-preselections)
  - [6.5 Running EEC calculations](#65-running-eec-calculations)
  - [6.6 NANO contents](#66-nano-contents)
- [7. Scaleout](#7-scaleout)
  - [7.1 Running with crab](#71-running-with-crab)
  - [7.2 Running with condor](#72-running-with-condor)


## 1. setup instructions

NB the version of the boost libraries that ships with CMSSW_10_26 does not have some of the functionality we need, so we have to trick the build system into using a local copy of boost 1.82.0. We only use the header-only libraries, so no need to actually build anything

```bash

# use central singularity environment
> cmssw-cc7

#setup CMSSW release area
> cmsrel CMSSW_10_6_26
> cd CMSSW_10_6_26/src/
> cmsenv
> git clone git@github.com:ssrothman/Cmssw-UserCode.git SRothman --branch EECs_106x

#setup usercode
> cd SRothman
> git submodule update --recursive --init

#setup boost library
> wget https://archives.boost.io/release/1.82.0/source/boost_1_82_0.tar.gz
> tar -xf boost_1_82_0.tar.gz #this takes a while. there is probably a better way to do this
> rm boost_1_82_0.tar.gz
```
Then, in a text editor:

edit $CMSSW_BASE/config/toolbox/slc7_amd64_gcc700/tools/selected/boost.xml such that:
 - BOOST_BASE = $CMSSW_BASE/src/SRothman/boost_1_82_0
 - INCLUDE = $BOOST_BASE
 - LIB = $BOOST_BASE
 - CMSSW_FWLITE_INCLUD_PATH = $BOOST_BASE

and similarly edit $CMSSW_BASE/config/toolbox/slc7_amd64_gcc700/tools/selected/boost_header.xml such that
 - BOSTHEADER_BASE = $CMSSW_BASE/src/SRothman/boost_1_82_0
 - INCLUDE = $BOOSTHEADER_BASE

Then, back in bash:

``` bash
#build
> cd $CMSSW_BASE/src
> scram setup # tell it to re-read the xml files and learn about the local copy of boost. 
              # When rebuilding later you don't need to do this
> scram b clean
> scram b -j8

#run
> cmsRun SRothman/Analysis/test/test_MC.py
```

## 2. Event selection

In order to optimize performance and disk requirements, only events that pass event selection are fully processed. However, we need to keep track of the total number of events in each dataset (eg for lumi weighting), so we actually produce two output files per event:
 - `NANO_selected.root` for events passing event selection, including the full needed information
 - `NANO_dropped.root` for events failing event selection, including only some basic gen-level info

Currently I have only implemented a basic `Z->mumu` event selection, including selection cuts on the muons and the resulting Z boson, and an optional MET veto. This is setup with:
```python
from SRothman.Analysis.setupEventSelections_cff import setupEventSelections
process = setupEventSelections(
  process, 
  muons : str, # InputTag for the muons
  config : dict, # configuration dict 
  isMC : bool,
  genmuons : bool # whether the muons are gen-level objects 
                  # (requires one different producer class, and no rochester corrections)
)
```

Changing the event selection is as easy as replacing the `setupEventSelections()` function with a different one that also creates a `process.selections_path` `cms.Path()` object. 

## 3. Jet building and preselection

CMSSW jet objects are setup as follows:
``` python
from SRothman.Analysis.setupAK8Jets_cff import setupAK8Jets
process = setupAK8Jets(process,
   isMC : bool, # whether to set up gen jets
   skipJTB : bool, # controls whether we need to make a new jet collection with the jet toolbox, 
                   # or if we can work off an existing jet collection
   genOnly : bool, # whether to skip setting up reco jets
   config : dict # configuration dictionary
)
```
This builds the AK8 PUPPI jets in CMSSW, calculates things like the standard jet ID fields, and sets up NANO tables for the jets. This also sets up some jet preselection logic, which is controlled by the config dictionary. 

For gen jets, the config dictionary must look like:
``` python
{
  "GenJets" : {
        "minPt" : float, #min pt. Negative numbers -> no cut
        "maxEta" : float, #max |eta|. Negative numbers -> no cut
        "maxMuEF" : float, # max muon energy fraction. number >=1 -> no cut
        "maxChEmEF" : float, # max charged EM fraction. number >=1 -> no cut
        "maxNEmEF" : float, # max neutral EM fraction. number >=1 -> no cut
        "maxNHadEF" : float, # max neutral hadron fraction. number >= 1 -> no cut
        "minConstituents" : int, # min number of constituents. Number <=0 -> no cut
        "muonVetoDR" : float, # veto jets within this Delta-R of one of the Z decay muons; <=0 for no cut
        "arbitration" : str, # jet arbitration mode. Options are:
                             #     'None" -> no jet arbitration
                             #     'Leading<N> -> keep only the leading <N> jets
  }
}
```
For reco jets, the config dictionary must look like:
```python
{
  "RecoJets" : {
        "minPt" : float,  #min pt. Negative numbers -> no cut
        "maxEta" : float, #max |eta|. Negative numbers -> no cut
        "jetID" : str, #either "none", "jetIdTight", or "jetIdLepVeto"
        "minConstituents" :  int, # min number of constituents. Number <=0 -> no cut
        "muonVetoDR" : float, # veto jets within this Delta-R of one of the Z decay muons; <=0 for no cut
        "arbitration" : "None" # jet arbitration mode. Options are:
                               #     'None" -> no jet arbitration
                               #     'Leading<N> -> keep only the leading <N> jets
  }
}
```

### 3.1 CMSSW products

This process modifier builds:

 - Gen jets with InputTag `arbitratedGenJetsAK8`
 - Reco jets with InputTag `selectedUpdatedJetsAK8`
 - Preselection `<jet -> bool>` ValueMaps corresponding to the reco jets with InputTags
     - `overlapVetoJetsAK8`
     - `preselectJetsAK8`

The preselection ValueMaps can be used downstream to avoid expensive calculations on jets that fail preselection (ie of EEC quantities)

### 3.2 NanoAOD products

In addition, this also sets up NanoAOD tables. 

The table for GenJets is named `arbitratedGenJetsAK8` and has branches
```python
pt : float # transverse momentum
eta : float # pseudorapidity
phi : float # azimuthal angle
mass : float # mass
area : float # jet area
nConstituents : int # number of jet constituents
chargedHadronEnergy : float # charged Hadron Energy
neutralHadronEnergy : float # neutral Hadron Energy
chargedEmEnergy : float # charged EM Energy
neutralEmEnergy : float # neutral EM Energy
muonEnergy : float # muon Energy
chargedHadronMultiplicity : int # charged Hadron Multiplicity
neutralHadronMultiplicity : int # neutral Hadron Multiplicity
chargedEmMultiplicity : int # charged EM Multiplicity
neutralEmMultiplicity : int # neutral EM Multiplicity
muonMultiplicity : int # muon Multiplicity
partonFlavour : int # parton flavour
hadronFlavour : int # hadron flavour
```

The table for reco jets is named `finalSelectedJetsAK8` and has branches
```python
pt : float # transverse momentum
eta : float # pseudorapidity
phi : float # azimuthal angle
mass : float # mass
area : float # jet area
jetIdLepVeto : int # jet ID (lep veto)
jetIdTight : int # jet ID (tight)
nConstituents : int # number of jet constituents
jecFactor : float # jec factor
chHEF : float # charged hadron energy fraction
neHEF : float # neutral hadron energy fraction
chEmEF : float # charged EM energy fraction
neEmEF : float # neutral EM energy fraction
muEF : float # muon energy fraction
hadronFlavour : int # hadron flavour
partonFlavour : int # parton flavour
nBHadrons : int # number of b hadrons
nCHadrons : int # number of c hadrons
overlapVeto : bool # overlap veto preselection flag
preselection : bool # kinematic preselection flag
```

## 4. SimonJets

All of my code works off a custom datatype `simon::jet` ("SimonJets"). These represent collections of jet constituents, with all collections and systematics "baked in".


### 4.1 Systematic variations

The SimonJets include any relevant systematic variations on the jet constituents "baked in". The particle selection is implemented in `SimonTools/src/particleSelector.h`, and includes:
 - An option to select only charged particles
 - An option to select only particles passing vertexing-based cuts (ie on the puppi weight, dxy, dz, and/or fromPV flag)
 - An option to vary particle momenta up and down by different factors for tracks, photons, and neutral hadrons
 - An option to randomly drop tracks and convert them into neutrals with smeared momenta
 - An option to apply minimum momentum thresholds separately for photons, neutral hadrons, electrons, muons, and charged hadrons

In the implementation, this behavior is controlled by two pieces: the so-called "parameters" which specify all of the relevant constants (eg energy scale uncertainties, cut values, ...), and the so-called "settings", which specifies what selections and modifications to actually run. The "parameters" are the same across all systematic variations, and which variation to take is controlled by which "settings" are passed. In order to reduce repetition in the configuration dict, the code expects a `"nominal"` settings dictionary with all settings full-specified, and then alternative settings can be specified under `"variations"` by only specifying those settings which are different from the nominal.

Concretely, this looks like:
```python
{
  "Systematics": {
    "parameters": {
      "EM0scale": float,  # EM energy scale variation 
      "HAD0scale": float,  # Hadronic energy scale variation
      "CHscale": float,  # Charged hadron scale variation
      "trkDropProb": float,  # Track dropout probability
      "trkDropSmear": float,  # Factor by which to smearing the energy of dropped tracks
      "EM0thresholds": [float, float, float],  # EM energy thresholds [NOM, UP, DN]
      "HAD0thresholds": [float, float, float],  # Hadronic energy thresholds [NOM, UP, DN]
      "ELEthresholds": [float, float, float],  # Electron thresholds [NOM, UP, DN]
      "MUthresholds": [float, float, float],  # Muon thresholds [NOM, UP, DN]
      "HADCHthresholds": [float, float, float],  # Charged hadron thresholds [NOM, UP, DN]
      "minFromPV": int,  # Minimum fromPV flag value
      "minPuppiWt": float,  # Minimum PUPPI weight
      "maxDZ": float,  # Maximum DZ
      "maxDXY": float  # Maximum DXY
    },
    "nominal": { # setup for nominal variation
      "EM0scale": str,  # Nominal EM scale ("NOM")
      "HAD0scale": str,  # Nominal HAD scale ("NOM")
      "CHscale": str,  # Nominal CH scale ("NOM")
      "trkDrop": str,  # Track dropout setting ("OFF")
      "EM0threshold": str,  # Nominal EM threshold ("NOM")
      "HAD0threshold": str,  # Nominal HAD threshold ("NOM")
      "ELEthreshold": str,  # Nominal ELE threshold ("NOM")
      "MUthreshold": str,  # Nominal MU threshold ("NOM")
      "HADCHthreshold": str,  # Nominal HADCH threshold ("NOM")
      "requireVertex": str,  # Vertex requirement ("ON")
      "applyPuppi": bool,  # Apply PUPPI weights
      "onlyCharged": bool  # Use only charged particles
    },
    "variations": { # modification of settings for systematic variations
      "variation_name_1" : {
        "setting_override_1" : "override value", # e.g. "EM0scale" : "UP",
        "settting_override_2" : "override value", # e.g. applyPuppi : false
        ...
      },
      "variation_name_2" : {
        ...
      },
      ...
    }
  }
}
```

### 4.2 SimonJets from CMSSW jets

There is a producer to build SimonJets from CMSSW jets at `CustomJets/plugins/SimonJetProducer.cc`. 


SimonJets can be set up from CMSSW jets with a process modifier as follows:
```python
from SRothman.CustomJets.setupSimonJets import setupSimonJets
process = setupSimonJets(process,
    jets : str, # InputTag name of reco jets
    genjets : str, #InputTag name of gen jets 
    CHSjets : str # (optionally) InputTag name of AK4 CHS jets, or "" to skip AK4 matching
    name : str, # name to use when building CMSSW producers and NANO tables
    syst : str, # systematic variation
    isMC : bool, # whether to build gen jets
    genOnly : bool # whether to not build reco jets
)
```

In addition to building the SimonJets, this producer also optionally produces a list of matching jets from the `CHSjets` collection. This can be skipped by passing an empty string as the `CHSjets` collection. The matching is one-to-many, accepting all CHS jets with a configurable delta-R of the SimonJet, controlled by
```python
{
  "Jets" : {
    "CHSmatchDR" : float
  }
}
```

### 4.3 NANO tables

There is also a producer to build NANO tables from SimonJets (`CustomJets/plugins/SimonJetTableProducer.cc`). This is automatically setup by the `setupSimonJets` function, and produces three tables:

1. The jet constituents table, which is named according to whatever was passed to `setupSimonJets()`. This includes branches
```python
pt : float # particle pt
eta : float # particle eta
phi : float # particle phi
pdgid : int # particle pdgid
charge : int # particle charge
vtx_x : float # vertex x coord
vtx_y : float # vertex y coord
vtx_z : float # vertex z coord
dxy : float # dxy
dz : float # dz
fromPV : int # from PV enum
puppiWeight : float # Puppi weight
```

2. The CHS jet match table, which is named `<name>CHS`. This has only one branch:
```python
idx : int # index of matched CHS jet
```

In order for unflattening by the number of CHS matches to work well with `awkward` in python, the number of entries per jet must always be at least one, so jets with zero CHS matches are padded with `idx=99999999`

3. The jet-level table, which is named `<name>BK` (BK standing for "Book-Keeping"). This has branches
```python
jetPt : float # jet pt
jetRawPt : float # raw jet pt
jecfactor : float # JEC factor
jetEta : float # jet eta
jetPhi : float # jet phi
iJet : int # index in primary jet array
jetMass : float # jet mass
nPart : int # number of particles in jet. Can be used to unflatten the constituents collection
nCHS : int # number of matched CHS jets. Can be used to unflatten the CHS match collection
```

By default `setupSimonJets()` also sets up an additional extension table for the `<name>BK` table, including the following extra branches:
```python
CHSpt : float # sum matched transverse momenta
CHSeta : float # eta of pt-weighted match centroid 
CHSphi : float # phi of pt-weighted match centroid
CHSparton : int # max of matched parton flavours
CHShadron : int # max of matched hadron flavours
CHSbtagDeepB : float # max of b-tagging discriminator (DeepB)
CHSbtagDeepFlavB : float # max of  b-tagging discriminator (DeepFlav B)
CHSbtagCSVV2 : float # max of  b-tagging discriminator (CSVV2)
CHSbtagDeepCvL : float # max of b-tagging discriminator (DeepCvL)
CHSbtagDeepCvB : float # max of b-tagging discriminator (DeepCvB)
CHSbtagDeepFlavCvL : float # max of b-tagging discriminator (DeepFlav CvL)
CHSbtagDeepFlavCvB : float # max of b-tagging discriminator (DeepFlav CvB)
CHSbtagDeepFlavQG : float # max of b-tagging discriminator (DeepFlav QG)
CHSqgl : float # max of quark/gluon likelihood
```

## 5. Gen-matching

The gen-matching is implemented in another backend submodule `Matching/src` and wrapped in a CMSSW producer `Matching/plugins/TrackMatchProducer.cc`. The current implementation only supports charged particles, and is best thought of as a greedy kinematic fit. The logic is as follows:

```
For each gen particle (descending in pT):
  1. Find all reco particles with deltaR < dR_threshold(reco pT)
  2. Identify the "best" match according to a chi^2 likelihood
           (Delta pT/sigma pT)^2 + (delta eta/sigma_eta)^2 + (delta phi/sigma_phi)^2
  3. Create the match, and remove the matched reco particle from contention
```

This can be set up with a process modifier:
```python
from SRothman.Matching.setupMatching import setupMatching
process = setupMatching(process,
    name : str, # name to use for the matching process
    reco : str, # reco SimonJets name
    gen : str, # gen SimonJets name
    config : dict # configuration dict
)
```

The matching algorithm is highly configurable, with the option to control the behavior differently for reco-level electrons, muons, and charged hadrons. The available parameters are:
```python
# ------- delta-R cone -----------
"dr_mode" : str,  # Delta-R matching mode 
"dr_param1" : float,  # Delta-R parameter 1
"dr_param2" : float,  # Delta-R parameter 2
"dr_param3" : float,  # Delta-R parameter 3


# ------ pT resolution ----------
"ptres_mode" : str,  # pT resolution mode ("ConstFrac", etc.)
"ptres_param1" : float,  # pT resolution parameter 1
"ptres_param2" : float,  # pT resolution parameter 2


# ----- angular resolution ------
"angres_mode" : str,  # Angular resolution mode ("TrackAng", etc.)
"angres_param1" : float,  # Angular resolution parameter 1
"angres_param2" : float,  # Angular resolution parameter 2

# ----- charge matching --------
"opp_charge_penalty" : float,  # Penalty for opposite charge matches
"no_charge_penalty" : float,  # Penalty for matching with neutrals
"charge_filter_mode" : str,  # Charge filtering mode ("Any", etc.)

# ----- flavor matching --------
"flavor_filter_mode" : str  # Flavor filtering mode ("Any", etc.)
```
### 5.1 Delta-R controls

The currently supported dr_modes are: 
1. `"Const"` - constant delta R cone size with radius `dr_param1`. The other two parameters are ignored
2. `"TrackPt"` - delta R cone size proportional to tracker resolution accounting for multiple scattering. The functional form is `A+B/pT`, with `A` = `dr_param1` and `B` = `dr_param2`. This is clipped to a maximum cone size given by `dr_param3`. 


### 5.2 Resolutions

The resolutions in the denominator of the chi^2 in the matching are parameterized by the reco particle kinematics. The avilable modes are:
1. `"Const"` - constant resolution given by `param1`, with `param2` ignored
2. `"ConstFrac"` - resolution is constant fraction of the reco pT, with the fraction given by `param1`, and `param2` ignored
3. `"TrackPt"` - resolution is parameterized according to standard track pT resolution function `param1 + param2 * pT`.
4. `"TrackAng"` - resolution is parameterized according to standard track angular resolution function `param1 + param2 / pT`

### 5.3 Charge matching

Three charge matching filters available:
1. `"Any"` - allow matches independent of charge
2. `"Magnitude"` - only allow matches with the same charge magnitude, independent of charge sign
3. `"Sign"` - only allow matches with the same charge sign (ie both neutral, both negative, or both positive)

If the charge matching filter is not strict, there is also an option to add penalty terms to the chi^2 for charge mismatches. These penalties are:
 - `opp_charge_penalty` for matches between charged particles with opposite sign
 - `no_charge_penalty` for matches between a charged particle and a neutral particle

### 5.4 Flavor matching

Several flavor matching filters are avilable:
1. `"Any"` - allow matches independent of flavor
2. `"AnyHadron"` - allow matches to any hadron (pdgId > 100)
3. `"AnyLepton"` - allow matches to any leptop (pdgid in [11, 13, 15])
4. `"Electron"` - allow matches only to electrons (pdgid 11)
5. `"Muon"` - allow matches only to muons (pdgid 13)
6. `"ElectronMuon"` - allow matches to either electrons or muons (pdgid in [11, 13])
7. `"Electromagnetic"` - allow matches to electrons, photons, and pi0s (pdgid in [11, 22, 111])
8. `"AnyCharged"` - allow matches to any charged flavor
9. `"AnyNeutral"` - allow matches to any neutral flavor
10. `"AnyChargedHadron"` - allow matches to any charged hadron (pdgId > 100 and charge!=0)
11. `"AnyNeutralHadron"` - allow matches to any neutral hadron (pdgId > 100 and charge==0)

### 5.5 NANO tables

The `TrackMatchTableProducer` creates extension tables for the gen and reco SimonJet tables, with extra branches relating to the matching. 

The reco-level jet constituents get the extra branches:
```python
matchPt : float # matched pT
matchEta : float # matched eta
matchPhi : float # matched phi
matchCharge : int # matched charge
nMatches : int # number of matched particles
matchTypes : int # matched particle type flag
                 # this is a bitmask:
                 # MATCH_MUON = 1;
                 # MATCH_ELE = 2;
                 # MATCH_PHO = 4;
                 # MATCH_HADCH = 8;
                 # MATCH_PI0 = 16;
                 # MATCH_HAD0 = 32;
```
and the global jet (`<name>BK`) gets the extra branches
```python
jetMatchPt : float # matched jet pT
jetMatchEta : float # matched jet eta
jetMatchPhi : float # matched jet phi
jetMatched : int # match boolean
iGen : int # index of matched gen jet
```

The gen-level jet constituents get the extra branches
```python
nMatches : int # number of matched particles
matchTypes : int # matched particle types bitmask
```
And the gen-level jet (`<name>BK`) gets the extra branches
```python
genJetMatched : int # match boolean
iReco : int # index of matched reco jet
```

## 6. EEC calculations

The actual EEC calculations are implemented in the EEC backend submodule `EECs/src`. These are wrapped in CMSSW producers in `EECs/plugins`, and NANO table producers are also provided. 

### 6.1 Types of EEC observables:

There are three different kinds of EEC calculators:

#### 6.1.1 Projected EECs (referred to throughout the code as "proj")

 By default this calculates the projected EECs from 2-point through 6-point. 
 These are 1-dimensional histograms, binned in delta R ("R" in the code).
 This coordinate lives in the domain `0 < R < \infty`

#### 6.1.2 Resolved three-point EECs (referred to throughout the code as "res3")

 This calculates the resolved three-point EECs. 
 This is a three-dimensional histogram. The binning is according to equation (3) of https://arxiv.org/pdf/2201.07800
 The names of the coordinates in the code are:
  - "R" = `RL`
  - "r" = `\xi`
  - "c" = `\phi`
  
and they live in the domains:
 - `0 < R < \infty` 
 - `0 < r < 1`
 - `0 < c < pi/2`

#### 6.1.3 Resolved four-point EECs (referred to throughout the code as "res4")

This calculates the resolved four-point EECs. 
In principle the full four-point EECs live in a five-dimensional space. This is practically infeasible, so instead we have picked out three three-dimensional subspaces of this space. These are: the "dipole", the "tee", and the "triangle". Each of these lives in a 3-dimensional space with coordinates naned `R`, `r`, and `c`

### 6.2 EEC calculator data formats ("resultType")

The EEC calculator can run in three different modes, yielding output with different amounts of pre-applied binning.

#### 6.2.1 Unbinned EECs (resultType="Unbinned")

This mode keeps the most information, at the cost of the most computational expense (both in terms of memory/cpu in the calculation, but also disk space required for the resulting NANO files). In this mode the EECs are completely unbinned (any bin edges passed in the configuration are ignored), and a distinct row is created in the NANO tables for each distinct N-tuple of jet constituents. 

Take, for example, the three-point projected EEC in a four-particle jet. There are four three-tuples of particles, plus six ways to take pairs of particles, and four ways to take a single particle. There will therefore be 14 rows in the NANO table, with the format:
```csv
R       wt
<float> <float>
<float> <float>
<float> <float>
...
```
Note that there will be repeated values in the `R` column because we are binning in only the "largest" `R` coordinate, which can be the same in different triples of particles. The trivial example of this is that when we take all three particles to be the same, the `R` coordinate will always be zero, so there will be four entries with `R=0`. 

#### 6.2.2 Prebinned, unmerged EECs (resultType="Vector")

In this mode, the EECs are pre-binned in the angular coordinates, but a distinct row is still created for each distinct N-tuple of particles. Here the binning passed in the config is used, and the output values for the angular coordinates are integer bin indices (indexed such that underflow is bin 0, the first bin is bin 1, etc). 

For the same example scenario as above, there will again be 14 rows in the NANO table, this time with a format like
```csv
R       wt
<int>   <float>
<int>   <float>
<int>   <float>
...
```

#### 6.2.3 Prebinned, merged EECs (resultType="Array")

This mode has the most aggressive optimization, and can give dramatically smaller files on disk than the other modes. This is achieved by pre-binning in the angular coordinates (just as for the "vector" resulttype), and merging entries which fall in the same bin. The output to the NANO table is the zero-suppressed total histogram counts in each angular bin. 

For the same example scenario as above, there are at most 7 distinct angular separations (0, A-B, A-C, A-D, B-C, B-D, C-D), so there will be at most 7 rows in the NANO table. If any of these angular separations fall in the same R bin, or if any potential R bins are empty, then there will be fewer rows. The maximum number of rows in the NANO table for a given jet is the total number of R bins in the supplied binning. The output will be in the same format as the other prebinned mode:
```csv
R       wt
<int>   <float>
<int>   <float>
<int>   <float>
...
```

### 6.3 EEC denominators

EEC entries are weighted by `pt_part / pt_jet`. There are in principle a few different options for what value to put for `pt_jet`, controlled by the `normType` parameter in the configuration. These options are:

 - `normType = NONE` : don't normalize by the jet pT. Equivalent to `pt_jet = 1`
 - `normType = SUMPT` : normalize by the sum of the momenta of all jet constituents passing the selections (eg charged-only, thresholds, etc)
 - `normType = RAWOT` : normalize by the sum of the momenta of all jet constituents, before selections (eg including neutrals, particle below thresholds, etc)
 - `normType = CORRPT` : normalize by the JEC-correct jet pT
 
 In any case the code tracks which value of `pt_jet` was used in the denominator, and writes this to the NANO, so post-hoc corrections or changes to this normalization can be applied trivially (this is needed, e.g. for JES/JER variations).

 ### 6.4 Jet preselections

 In order to avoid running expensive calculations of jets that fail selections, the EEC producers accept a list of preselection flags (such as the OverlapVeto and Preselection flags discussed when building CMSSW jets). If a jet fails any of the passed flags, the EEC calculations are skipped.

### 6.5 Running EEC calculations

A generic process modifier for setting up EEC calculations in data or MC is avilable in `SRothman/EECs/setupEEC.py`. This can be called as
``` python
from SRothman.EECs.setupEEC import setupEEC_MC
process = setupEEC_MC(process,
    name : str = "name to use in CMSSW (eg for naming NANO tables)",
    genMatch : str = "name (ie input tag) of genmatching producer",
    genjets : str = 'name (ie input tag) of simonjets producer for gen jets',
        config : dict = "configuration dictionary",
    recojets : str= 'name (ie input tag) of simonjets producer for gen jets',
    whichEEC : str = "one of 'proj', 'res3', 'res4'",
    verbose : int = "verbosity level (0 or 1 basically)",
)
```
or 
``` python
from SRothman.EECs.setupEEC import setupEEC_data
process = setupEEC_data(process,
    name : str = "name to use in CMSSW (eg for naming NANO tables)",
    recojets : str= 'name (ie input tag) of simonjets producer for gen jets',
    config : dict = "configuration dictionary",
    whichEEC : str = "one of 'proj', 'res3', 'res4'",
    verbose : int = "verbosity level (0 or 1 basically)",
)
```

This automatically sets up the correct CMSSW producers, and names everything consistently such that there are no name clashes. Note that it is possible to run multiple EEC calculations at once, just by calling the process modifier multiple times with different `whichEEC` values. An example of this can be found in `Analysis/test/test_MC.py` or `Analysis/test/test_data.py`.

### 6.6 NANO contents

The EEC table producers write two NANO tables. One is just the list of EEC entries, in the format
```csv
R  (and maybe r, c)  wt
<float or int>       <float>
<float or int>       <float>
<float or int>       <float>
...
```
where the angular coordinate is either floating-point (for Unbinned calculators) or integer bin indices (for Vector or Array calculators). 

In addition, there is also a "bookkeeping table" with name `<name>BK`. This has fields
```python
nR: int #number of R bins. For unbinned calculators this still exists, but is meaningless
(nr, nc) : int # same as nR for r and c in resolved calculators
nEntry : int # number of entries per jet. Can be used to unflatten the other table
pt_denom: float # the value of pt_jet used in the denominator for EEC calculation
```

For EEC transfer matrices, the tables are very similar. The main table looks like
```csv
R_gen                 wt_gen      R_reco               wt_reco
<float or int>       <float>      <float or int>       <float>
<float or int>       <float>      <float or int>       <float>
<float or int>       <float>      <float or int>       <float>
...
```
(potentially with also r_gen, r_reco, c_gen, c_reco)
and the `BK` (bookkeeping) table has
```python
nR_gen: int #number of R bins. For unbinned calculators this still exists, but is meaningless
nR_reco: int #number of R bins. For unbinned calculators this still exists, but is meaningless
(nr_gen, nr_reco, nc_gen, nc_reco) : int # same as nR for r and c in resolved calculators
nEntry : int # number of entries per jet. Can be used to unflatten the other table
pt_denom_reco: float # the value of pt_jet used in the denominator for EEC calculation
pt_denom_gen: float # the value of pt_jet used in the denominator for EEC calculation
```

## 7. Scaleout

### 7.1 Running with crab

First, ensure that RUNNING_CRAB = True in Analysis/python/common_cmsRun.py

Then, cd Analysis/production/crab/

edit production_tag.py to be some unique name 

edit submit.py to point to an allocation you have at some T2 or T3

create a .yaml file with the datasets, config files, global tag, and crab config you want to use

run
```bash
> python submit.py -y <your yaml>
```
### 7.2 Running with condor

There is a framework for running on condor in Analysis/production/condor. 
