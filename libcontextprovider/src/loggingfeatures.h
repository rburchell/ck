/*
 * Copyright (C) 2008, 2009 Nokia Corporation.
 *
 * Contact: Marius Vollmer <marius.vollmer@nokia.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef FEATURES_H
#define FEATURES_H

#define F_SERVICE           (ContextFeature("service"))
#define F_SERVICE_BACKEND   (ContextFeature("servicebackend"))
#define F_PROPERTY          (ContextFeature("property"))
#define F_MANAGER           (ContextFeature("manager"))
#define F_SUBSCRIBER        (ContextFeature("subscriber"))
#define F_GROUP             (ContextFeature("group"))
#define F_DESTROY           (ContextFeature("destroy"))
#define F_SIGNALS           (ContextFeature("signals"))
#define F_DBUS              (ContextFeature("dbus"))
#define F_C                 (ContextFeature("cbindings"))

#endif
