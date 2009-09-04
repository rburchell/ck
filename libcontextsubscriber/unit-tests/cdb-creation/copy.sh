#!/bin/bash -e

cp simple/cache.cdb ../contextpropertyinfo-cdb-static/cache.cdb
cp simple/cache.cdb ../contextregistryinfo-cdb-static/cache.cdb

cp dynamic/providers2v1/cache.cdb ../contextpropertyinfo-cdb-dynamic/context-providers2v1.cdb
cp dynamic/providers2v2/cache.cdb ../contextpropertyinfo-cdb-dynamic/context-providers2v2.cdb

cp dynamic/providers2v1/cache.cdb ../contextpropertyinfo-cdb-dynamic/context-providers3v1.cdb
cp dynamic/providers3v2/cache.cdb ../contextpropertyinfo-cdb-dynamic/context-providers3v2.cdb

cp dynamic/providers2v1/cache.cdb ../contextpropertyinfo-cdb-dynamic/context-providers4v1.cdb
cp dynamic/providers4v2/cache.cdb ../contextpropertyinfo-cdb-dynamic/context-providers4v2.cdb



cp dynamic/providers2v1/cache.cdb ../contextregistryinfo-cdb-dynamic/context-providers1v1.cdb
cp dynamic/providers1v2/cache.cdb ../contextregistryinfo-cdb-dynamic/context-providers1v2.cdb

