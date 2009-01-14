using GLib;

public class Main
{	
	ContextProvider.LibraryMain lib;
	public void run () {
		var loop = new MainLoop (null, false);
		stdout.printf ("Started main loop\n");
		lib = new ContextProvider.LibraryMain();
		lib. start ();
		loop.run();
	}

	static int main (string[] args) {
		var main = new Main ();
		main.run ();
		return 0;
	}
}
