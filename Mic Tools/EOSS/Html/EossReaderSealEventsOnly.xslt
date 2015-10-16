<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
				xmlns:msxsl="urn:schemas-microsoft-com:xslt"
                xmlns:eoss="eoss"
				version="1.0">
<msxsl:script language="JScript" implements-prefix="eoss">
	<![CDATA[
	
	var Operator1="";
	var Operator2="";
	
    function Unattended()
    {
		if ((Operator1 == "") && (Operator2 == ""))
			return 1;
		else
			return 0;
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
    
    function Operator1Logon(Code)
    {
		Operator1 = ToId(Code);
		
		return "";
    }
    
    function Operator2Logon(Code)
    {
		Operator2 = ToId(Code);
		
		return "";
    }
    
    function OperatorsLogoff(Code)
    {
		Operator1 = "";
		Operator2 = "";
		
		return "";
    }
    
    function ByWhom()
    {
		
		if ((Operator1 != "") && (Operator2 != ""))
			return Operator1 + " and " + Operator2;
						
		if (Operator1 != "")
			return Operator1;

		if (Operator2 != "")
			return Operator2;
			
		return "Unknown";
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
 <xsl:if test="eoss:Unattended()">
  <TD align="middle"><img src="excl.gif" alt="Occured in absence of an authorized operator."/></TD>
  <TD align="left" class="detail"><xsl:value-of select="@date"/>&#32;<xsl:value-of select="@time"/></TD>
  <TD class="detail">
  <xsl:if test="(@index mod 2) != 0">Seal wire opened.</xsl:if>
  <xsl:if test="(@index mod 2) = 0">Seal wire closed.</xsl:if>
  </TD>
 </xsl:if> 
 <xsl:if test="(eoss:Unattended()=0)">
  <TD align="middle"></TD>
  <TD align="left" class="detail"><xsl:value-of select="@date"/>&#32;<xsl:value-of select="@time"/></TD>
  <TD class="detail">
  <xsl:if test="(@index mod 2) != 0">Seal wire opened by </xsl:if>
  <xsl:if test="(@index mod 2) = 0">Seal wire closed by </xsl:if>
  <xsl:value-of select="eoss:ByWhom()"/>.
  </TD>
  </xsl:if> 
 </TR>
</xsl:if>
</xsl:template>

<xsl:template match="page/plain[@log='sys']/event">
<xsl:choose>
<xsl:when test="((@code &gt; 0) and (@code &lt; 19683))"><xsl:value-of select="eoss:Operator1Logon(number(@code))"/>
</xsl:when>
<xsl:when test="((@code &gt; 20480) and (@code &lt; 40163))"><xsl:value-of select="eoss:Operator2Logon(number(@code) - 20480)"/>
</xsl:when>
<xsl:when test="@code=40960"><xsl:value-of select="eoss:OperatorsLogoff()"/>
</xsl:when>
</xsl:choose>  
</xsl:template>

</xsl:stylesheet>

  