from coffea.nanoevents import NanoEventsFactory, NanoAODSchema
import awkward as ak
import numpy as np

x = NanoEventsFactory.from_root("NANO_NANO.root", schemaclass=NanoAODSchema).events()

