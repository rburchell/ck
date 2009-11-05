check-customer:
	$(MAKE) -C @abs_top_srcdir@ all
	./test.sh

clean-local:
	rm -f *.actual *.cdb

EXTRA_DIST = test.sh regen.sh						\
	Battery.Charging_KEYDOC.expected				\
	Battery.Charging_KEYTYPE.expected                               \
	KEYS.expected							\
	providers1.context providers2.context				\
	Battery.Charging_PROVIDERS.expected
