providerdir = $(datadir)/context-providers/
dist_provider_DATA = fake-provider.xml

contextkitproviderdir = $(datadir)/contextkit/providers/
dist_contextkitprovider_DATA = com.nokia.FakeContextProvider.xml

install-data-hook:
	-update-context-providers $(providerdir)
	-update-contextkit-providers $(contextkitproviderdir)
