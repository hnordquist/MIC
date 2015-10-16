<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
				xmlns:msxsl="urn:schemas-microsoft-com:xslt"
                xmlns:eoss="eoss"
				version="1.0">

<xsl:output method="xml"/>

<xsl:template match="key">
 <xsl:element name="fo">
 <xsl:apply-templates select="page/plain[@log='fo']/event">
 <xsl:sort order="descending" select="@date"/>
 <xsl:sort order="descending" select="@time"/>
 <xsl:with-param name="inidate" select="@date"/>
 <xsl:with-param name="initime" select="@time"/>
 </xsl:apply-templates>
 </xsl:element>
</xsl:template>

<xsl:template match="page/plain[@log='fo']/event">
 <xsl:param name="inidate"/>
 <xsl:param name="initime"/>
 <xsl:if test="((@date != $inidate) or (@time != $initime))">
  <xsl:element name="event">
  <xsl:if test="(@index mod 2) != 0">Seal wire opened on <xsl:value-of select="@date"/> at <xsl:value-of select="@time"/>.</xsl:if>
  <xsl:if test="(@index mod 2) = 0">Seal wire closed on <xsl:value-of select="@date"/> at <xsl:value-of select="@time"/>.</xsl:if>
  </xsl:element>
 </xsl:if>
</xsl:template>

</xsl:stylesheet>

  