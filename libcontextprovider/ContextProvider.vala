namespace ContextProvider {

	// Definitions of callback function types
	public delegate void GetCallback(StringSet keys, void* values);
	// FIXME: use int (hashtable index) instead of void*
	public delegate void SubscribeCallback(StringSet keys);

	public int init ([CCode (array_length = false, array_null_terminated = true)] string[] provided_keys, GetCallback get_cb, SubscribeCallback first_cb, SubscribeCallback last_cb) {
		Manager manager = Manager.get_instance ();
		
		CProvider provider = new CProvider(get_cb, first_cb, last_cb);
		manager.providers.register (provided_keys, provider);
		return 1;
	}

	public int no_of_subscribers (string key) {
		Manager manager = Manager.get_instance ();
		return manager.key_counter.number_of_subscribers (key);
	}
}

