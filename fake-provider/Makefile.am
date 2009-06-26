providerdir = $(datadir)/contextkit/providers/
oldproviderdir = $(datadir)/context-providers/

dist_provider_DATA = com.nokia.FakeContextProvider.context
dist_oldprovider_DATA = fake-provider.xml

install-data-hook:
	-update-context-providers $(DESTDIR)$(oldproviderdir)
	-update-contextkit-providers $(DESTDIR)$(providerdir)

uninstall-hook:
	-update-context-providers $(DESTDIR)$(oldproviderdir)
	-update-contextkit-providers $(DESTDIR)$(providerdir)
	[ 0 -lt `find $(DESTDIR)$(oldproviderdir) -name '*.xml' | wc -l` ] || rm -f $(DESTDIR)$(oldproviderdir)/context-providers.cdb
	[ 0 -lt `find $(DESTDIR)$(providerdir) -name '*.context' | wc -l` ] || rm -f $(DESTDIR)$(providerdir)/cache.cdb
