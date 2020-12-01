Forward compatibility with the “implements" statement
=====================================================

The “implements" statement is a feature that is intended to help with future compatibility and variation of service
object types. The concept behind “implements" is to state that one object type “covers" another object type. This means
that it contains all the members with matching names, types, and parameters. Unlike other languages like Python there is
no implicit inheritance of members; each member must be specified in the object that contains the implements statement.
The idea behind the implements statement is to allow a client to use a service object that has more features than it
understands. For example, consider a simple service “Light" that contains an object with one member, “Power" which can
either be 0 or 1. Now consider another service “AdvancedLight" with an object that contains two members, “Power" and
“Dim" but implements “Light". A client that understands “Light" can still use “AdvancedLight" because it implements the
simpler “Light". This is less of an issue in Python because there is no explicit typing, but in other languages this can
become very important. The implements statement must form a clear hierarchy; there cannot be circular implements.
Implements can be used with the “import" statement the same way that structures and objrefs can work with the “import"
statement. This behavior is called "polymorphism".
