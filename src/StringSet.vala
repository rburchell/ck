namespace ContextKit {

	/* TODO:
	   we should have a way of statically initialising any keys that may be used in a stringset
	   so keys outside of this set are rejected
	   */
	/**
	 StringSet builds upon IntSet and Quarks to give us an structure which 
	 can manage sets of strings and efficiently perform set operations such
	  as intersection and union.
	 */
	public class StringSet {
		IntSet intset;

		StringSet.from_intset (IntSet #i) {
			intset = #i;
		}

		public StringSet () {
			intset = new IntSet();
		}

		public StringSet.sized (uint size) {
			intset = new IntSet.sized (size);
		}

		public StringSet.from_array (string[] array) {
			foreach (var s in array) {
				add (s);
			}
		}

		public StringSet.intersection (ContextKit.StringSet left, ContextKit.StringSet right) {
			this.from_intset (new IntSet.intersection (left.intset, right.intset));
		}

		public StringSet.symmetric_difference (ContextKit.StringSet left, ContextKit.StringSet right) {
			this.from_intset (new IntSet.symmetric_difference (left.intset, right.intset));
		}

		public StringSet.difference (ContextKit.StringSet left, ContextKit.StringSet right) {
			this.from_intset (new IntSet.difference (left.intset, right.intset));
		}

		public StringSet.union (ContextKit.StringSet left, ContextKit.StringSet right) {
			this.from_intset (new IntSet.union (left.intset, right.intset));
		}

		public void add (string element) {
			intset.add (Quark.from_string (element));
		}

		public void clear () {
			intset.clear ();
		}
		/* TODO, not sure how @foreach functions actually work :/
		public void @foreach (ContextKit.IntFunc func, void* userdata) {
		}
		*/
		public bool is_member (string element) {
			Quark q = Quark.try_string (element);
			if (q != 0)
				return intset.is_member (q);
			else
				return false;
		}

		public bool is_equal (ContextKit.StringSet right) {
			return intset.is_equal (right.intset);
		}

		public bool is_subset (ContextKit.StringSet right) {
			return intset.is_subset (right.intset);
		}

		public bool is_disjoint (ContextKit.StringSet right) {
			return intset.is_disjoint (right.intset);
		}

		public bool remove (string element) {
			Quark q = Quark.try_string (element);
			if (q != 0)
				return intset.remove (q);
			else
				return false;
		}

		public uint size () {
			return intset.size();
		}
	}
}
