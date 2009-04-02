namespace ContextProvider {
	public class Group {
		public SubscriptionChangedCallback? callback;
		public StringSet keys;
		bool subscribed;

		public Group (string[] keys, SubscriptionChangedCallback? cb) {
			this.keys = new StringSet.from_array(keys);
			this.callback = cb;
			this.subscribed = false;
		}
		public void subscribe (bool subscribed) {
			if (subscribed != this.subscribed) {
				this.subscribed = subscribed;
				if (callback != null)
					callback (subscribed);
			}
		}
	}
}
