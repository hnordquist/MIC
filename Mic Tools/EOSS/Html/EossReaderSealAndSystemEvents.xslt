<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
				xmlns:msxsl="urn:schemas-microsoft-com:xslt"
                xmlns:eoss="eoss"
				version="1.0">
				
<msxsl:script language="JScript" implements-prefix="eoss">
	<![CDATA[
	var LoggedOff=false;
    function Unattended(Code)
    {
      if ((Code > 0) && (Code < 19683))
         LoggedOff = false;
      if ((Code > 20480) && (Code < 40163))
         LoggedOff = false;
      if (Code == 40960)
         LoggedOff = true;
		return (LoggedOff && (Code != 40960));
    }
    function ToId(Code)
    {
      var Digit;
      var Id = "";

      while (Code) {
         Digit = Code % 27;
         if (Digit)
            Id = String.fromCharCode(Digit + 64) + Id;
         else
            Id = " " + Id;
         Code = Code / 27;
         Code = Math.floor(Code);
      }
      return (Id);
    }
    ]]>
</msxsl:script>

<xsl:output method="html"/>

<xsl:template match="key">
<TABLE id="EventTable" width="100%" cellSpacing="0" cellPadding="1">
<xsl:apply-templates select="page/plain/event">
 <xsl:sort select="@date"/>
 <xsl:sort select="@time"/>
 <xsl:with-param name="inidate" select="@date"/>
 <xsl:with-param name="initime" select="@time"/>
</xsl:apply-templates>
</TABLE>
</xsl:template>

<xsl:template match="page/plain[@log='fo']/event">
<xsl:param name="inidate"/>
<xsl:param name="initime"/>
<xsl:if test="((@date != $inidate) or (@time != $initime))">
 <TR class="info">
 <TD align="middle"><xsl:if test="eoss:Unattended(0)"><img src="excl.gif" alt="Occured in absence of an authorized operator."/></xsl:if></TD>
 <TD align="left" class="detail"><xsl:value-of select="@date"/>&#32;<xsl:value-of select="@time"/></TD>
 <TD class="detail">
 <xsl:if test="(@index mod 2) != 0">Seal wire opened.</xsl:if>
 <xsl:if test="(@index mod 2) = 0">Seal wire closed.</xsl:if>
 </TD>
 </TR>
</xsl:if>
</xsl:template>

<xsl:template match="page/plain[@log='sys']/event">
<TR class="info">
<TD align="middle"><xsl:if test="eoss:Unattended(number(@code))"><img src="excl.gif" alt="Occured in absence of an authorized operator."/></xsl:if></TD>
<TD align="left" class="detail"><xsl:value-of select="@date"/>&#32;<xsl:value-of select="@time"/></TD>
<TD class="detail">
<xsl:choose>
<xsl:when test="((@code &gt; 0) and (@code &lt; 19683))"><xsl:value-of select="eoss:ToId(number(@code))"/>&#32;logged on as operator 1.
</xsl:when>
<xsl:when test="((@code &gt; 20480) and (@code &lt; 40163))"><xsl:value-of select="eoss:ToId(number(@code) - 20480)"/>&#32;logged on as operator 2.
</xsl:when>
<xsl:when test="@code=40960">Operator(s) logged off.
</xsl:when>
<xsl:when test="@code=40976">Date and time set.</xsl:when>
<xsl:when test="@code=40977">Date and time set.</xsl:when>
<xsl:when test="@code=40992">Battery compartment opened.</xsl:when>
<xsl:when test="@code=40993">Battery compartment closed.</xsl:when>
<xsl:when test="@code=41008">Seal tampered with. Key-set erased.</xsl:when>
<xsl:when test="@code=41024">Access denied. Authorization failed.</xsl:when>
<xsl:when test="@code=41040">New key-set generated.</xsl:when>
<xsl:when test="@code=41056">Reset.</xsl:when>
<xsl:when test="@code=41072">Key-set erased on operator request.</xsl:when>
<xsl:when test="@code=41073">Temperator out of range. Key-set erased.</xsl:when>
<xsl:when test="@code=41088">Entering deep sleep mode.</xsl:when>
<xsl:otherwise>
Code&#32;<xsl:value-of select="@code"/>
</xsl:otherwise>
</xsl:choose>
</TD>
</TR>
</xsl:template>

</xsl:stylesheet>

  