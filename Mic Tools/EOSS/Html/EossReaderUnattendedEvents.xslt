<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
				xmlns:msxsl="urn:schemas-microsoft-com:xslt"
                xmlns:eoss="eoss"
				version="1.0">

<msxsl:script language="JScript" implements-prefix="eoss">
	<![CDATA[
	var LoggedOff=false;
	var UnattendedEvents = 0;
    function CountUnattended(Code)
    {
      if ((Code > 0) && (Code < 19683))
         LoggedOff = false;
      if ((Code > 20480) && (Code < 40163))
         LoggedOff = false;
      if (Code == 40960)
         LoggedOff = true;
		if (LoggedOff && (Code != 40960))
			UnattendedEvents++;
		return UnattendedEvents;
    }
    ]]>
</msxsl:script>

<xsl:output method="html"/>

<xsl:template match="key">
<xsl:apply-templates select="page/plain/event">
 <xsl:sort select="@date"/>
 <xsl:sort select="@time"/>
</xsl:apply-templates>
<xsl:value-of select="eoss:CountUnattended(1)"/>
</xsl:template>

<xsl:template match="page/plain[@log='fo']/event">
<xsl:if test="eoss:CountUnattended(0)"></xsl:if> 
</xsl:template>

<xsl:template match="page/plain[@log='sys']/event">
<xsl:if test="eoss:CountUnattended(number(@code))"></xsl:if>
</xsl:template>

</xsl:stylesheet>

  