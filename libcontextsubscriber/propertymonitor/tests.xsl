<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet xmlns:prov="http://contextkit.freedesktop.org/Provider" version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="xml" indent="yes" encoding="UTF-8"/>
<xsl:template match="/">
<testdefinition version="0.1">
  <suite name="contextfw-property-monitor">
    <set name="core" description="core properties">
      <pre_steps>
        <step expected_result="0">/usr/share/propertymonitor-tests/waitforsilence</step>
      </pre_steps>
      <xsl:for-each select="prov:properties/prov:key">
	<case requirement="" timeout="20">
	  <xsl:attribute name="name"><xsl:value-of select="@name"/>-provider</xsl:attribute>
	  <xsl:attribute name="description">Provider for <xsl:value-of select="@name"/></xsl:attribute>
          <step expected_result="0">. /tmp/session_bus_address.user;cd /usr/share/propertymonitor-tests;python /usr/share/propertymonitor-tests/has_provider.py <xsl:value-of select="@name"/></step>
	</case>
	<case requirement="" timeout="20">
	  <xsl:attribute name="name"> <xsl:value-of select="@name"/>-value</xsl:attribute>
	  <xsl:attribute name="description">Value for <xsl:value-of select="@name"/></xsl:attribute>
          <step expected_result="0">. /tmp/session_bus_address.user;cd /usr/share/propertymonitor-tests;python /usr/share/propertymonitor-tests/has_value.py <xsl:value-of select="@name"/></step>
	</case>
      </xsl:for-each>
      <environments>
        <scratchbox>false</scratchbox>
        <hardware>true</hardware>
      </environments>
      <get>
        <file>/tmp/xyzzy.log</file>
      </get>
    </set>
  </suite>
</testdefinition>
</xsl:template>
</xsl:stylesheet>
