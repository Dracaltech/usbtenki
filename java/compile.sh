#!/bin/bash -x

JAVA_HOME=/opt/jdk1.6.0_06/

JAVAC=$JAVA_HOME/bin/javac
JAVADOC=$JAVA_HOME/bin/javadoc

$JAVAC com/raphnet/tenki/*.java Test.java
$JAVADOC -classpath . -d ./javadoc  com.raphnet.tenki 
jar cf jusbtenki.jar com/

