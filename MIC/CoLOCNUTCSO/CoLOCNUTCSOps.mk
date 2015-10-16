
CoLOCNUTCSOps.dll: dlldata.obj CoLOCNUTCSO_p.obj CoLOCNUTCSO_i.obj
	link /dll /out:CoLOCNUTCSOps.dll /def:CoLOCNUTCSOps.def /entry:DllMain dlldata.obj CoLOCNUTCSO_p.obj CoLOCNUTCSO_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del CoLOCNUTCSOps.dll
	@del CoLOCNUTCSOps.lib
	@del CoLOCNUTCSOps.exp
	@del dlldata.obj
	@del CoLOCNUTCSO_p.obj
	@del CoLOCNUTCSO_i.obj
