using GLib;
using Gee;

public HashMap <string, TestDelegate> delegs;
public DBus.Connection conn;

[DBus (name = "org.gnome.TestDelegate")]
public class TestDelegate : Object {
    int64 counter;

    public int64 ping (string msg) {
        message ("%s", msg);
        return counter++;
    }
}

[DBus (name = "org.gnome.TestServer")]
public class TestServer : Object {
        public void GetDelegate (DBus.BusName sender, out DBus.ObjectPath path) {
                debug("GetDelegate");
                debug(sender);
                var deleg = new TestDelegate ();

                path = new DBus.ObjectPath ("/org/gnome/delegate/"+delegs.size.to_string());
                conn.register_object (path, deleg);

                message ("store:");
                message (sender);

                delegs.set (sender, deleg);
        }

        public void Unregister (DBus.ObjectPath path) {
                delegs.remove (path);
        }
}

void on_client_lost (DBus.Object sender, string name, string prev, string newp) {
        debug("on_client_lost");
        debug(name);
        debug(prev);
        debug(newp);
        // We lost a client
        delegs.remove (prev);
}

void main () {
    var loop = new MainLoop (null, false);

    try {
        conn = DBus.Bus.get (DBus.BusType. SESSION);

        delegs = new HashMap <string, TestDelegate> ();

        dynamic DBus.Object bus = conn.get_object ("org.freedesktop.DBus",
                                                   "/org/freedesktop/DBus",
                                                   "org.freedesktop.DBus");
        
        // try to register service in session bus
        uint request_name_result = bus.request_name ("org.freedesktop.ContextKit", (uint) 0);
       
        // client   
        int i = 0;
     
        dynamic DBus.Object server_object = conn.get_object ("org.freedesktop.ContextKit",
                                                             "/org/freedesktop/ContextKit/Manager",
                                                             "org.freedesktop.ContextKit.Manager");

        server_object.GetSubscriber();

    } catch (Error e) {
        stderr.printf ("Client error: %s\n", e.message);
    }
}
