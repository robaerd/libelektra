import Elektra.*;

/** Simple hello world example how Elektra could be wrapped to java. */
public class HelloElektra {

	public static void main(String[] args) {
		Key key = Key.create("user/hello_world",
			Key.KEY_VALUE, "Hello World",
			Key.KEY_END);
		System.out.println(key.name());
		System.out.println(key.string());
		key.print();


		KeySet ks = KeySet.create(10,
			Key.create("user/hello_world2",
				Key.KEY_VALUE, "Hello World2",
				Key.KEY_END).get(),
			KeySet.KS_END);
		/*
		KDB kdb = KDB.open(key);
		kdb.get(ks, key);
		Key k = ks.lookup(key);
		System.out.println(k.string());
		*/

		PluginDemo dp = new PluginDemo();
		dp.open(key);
		dp.get(ks, key);
		dp.close(key);
	}
}
