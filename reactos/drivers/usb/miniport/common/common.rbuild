<module name="usbminiportcommon" type="objectlibrary">
	<define name="__USE_W32API" />
	<include>../linux</include>
	<include base="usbport">.</include>
	<file>fdo.c</file>
	<file>main.c</file>
	<file>misc.c</file>
	<file>pdo.c</file>
</module>
