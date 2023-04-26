from coffea.nanoevents import NanoEventsFactory, NanoAODSchema

x = NanoEventsFactory.from_root("NANO_NANO.root", schemaclass=NanoAODSchema).events()
