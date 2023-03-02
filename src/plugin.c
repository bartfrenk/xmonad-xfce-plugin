/*  $Id$
 *
 *  Copyright (C) 2023 John Doo <john@foo.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "plugin.h"

#include <pygobject.h>
#include <dlfcn.h>
#include <config.h>

static void
sample_py_construct(XfcePanelPlugin *plugin);

XFCE_PANEL_PLUGIN_REGISTER(sample_py_construct);

static SamplePyPlugin *
sample_py_new(XfcePanelPlugin *plugin)
{
    SamplePyPlugin *sample_py;

    PyObject *py_widget_module_name; // Plugin module name as python object to import
    PyObject *py_widget_dict;        // Python context dictionary
    PyObject *py_widget_module;      // Holds the imported widget module
    PyObject *py_widget_class;       // Our plugin class

    // allocating space for our plugin
    sample_py = g_slice_new0(SamplePyPlugin);
    sample_py->plugin = plugin;

    // Needed, otherwise importing gi on the Python side crashes due to missing Python symbols.
    // E.g, PyUnicode_FromFormat. This happens, because xfce4-panel is not linked against Python.
    // So even if this .so is linked against it, symbols do not propagate through the callchain from
    // panel -> plugin.so -> plugin.py -> gi -> _gi.cpython-310-x86_64-linux-gnu.so
    // Usually, C-extensions in python aren't linked against python (for reasons).
    // dlopen here with RTLD_GLOBAL fixes that.
    // https://gitlab.xfce.org/itsManjeet/xfce4-python-sample-plugin/-/merge_requests/2
    sample_py->library_handler = dlopen(LIBPYTHON_NAME, RTLD_LAZY | RTLD_GLOBAL);
    if (sample_py->library_handler == NULL)
    {
        fprintf(stderr, "Error: %s\n", dlerror());
        return NULL;
    }

    // Initializing Python Interpreter
    Py_Initialize();

    // Adding python module search path
    PyObject *sys_path = PySys_GetObject((char *)"path");
    PyList_Append(sys_path, PyUnicode_FromString(PYTHON_SEARCH_PATH));

    // Generating module name as python string
    py_widget_module_name = PyUnicode_FromString(PLUGIN_ID);
    if (py_widget_module_name == NULL)
    {
        PyErr_Print();
        return NULL;
    }

    // Importing python plugin code as module
    py_widget_module = PyImport_Import(py_widget_module_name);
    if (py_widget_module == NULL)
    {
        PyErr_Print();
        return NULL;
    }
    Py_DECREF(py_widget_module_name);

    // Getting python context dictionary
    py_widget_dict = PyModule_GetDict(py_widget_module);
    if (py_widget_dict == NULL)
    {
        PyErr_Print();
        return NULL;
    }
    Py_DECREF(py_widget_module);

    // Getting "PanelPlugin" class from context
    py_widget_class = PyDict_GetItemString(py_widget_dict, "PanelPlugin");
    if (py_widget_class == NULL)
    {
        PyErr_Print();
        return NULL;
    }
    Py_DECREF(py_widget_dict);

    // Verifying class
    if (PyCallable_Check(py_widget_class))
    {
        // creating python object from the initializing class
        sample_py->py_object = PyObject_CallObject(py_widget_class, NULL);
    }
    else
    {
        PyErr_Print();
        fprintf(stderr, "Entry is not Python class");
    }
    Py_DECREF(py_widget_class);

    return sample_py;
}

static void
sample_py_orientation_changed(XfcePanelPlugin *plugin,
                              GtkOrientation orientation,
                              SamplePyPlugin *sample_py)
{
    // TODO: can we pass GtkOrientation as py Gtk.Orientation.<>
    PyObject_CallMethod(sample_py->py_object, "orientation_changed", "(i)", orientation == GTK_ORIENTATION_HORIZONTAL ? 0 : 1);
}

static void
sample_py_free(XfcePanelPlugin *plugin,
               SamplePyPlugin *sample_py)
{

    // calling 'free' method from python object,
    // Most probably ignored if not exist
    PyObject_CallMethod(sample_py->py_object, "free", "");

    // freeing python object and finalizing interpreter
    Py_DECREF(sample_py->py_object);
    Py_Finalize();

    // close the library handler
    dlclose(sample_py->library_handler);

    g_slice_free(SamplePyPlugin, sample_py);
}

static void
sample_py_about(XfcePanelPlugin *plugin,
                SamplePyPlugin *sample_py)
{
    // calling 'about' method from python object,
    PyObject_CallMethod(sample_py->py_object, "about", "");
}

static void
sample_py_construct(XfcePanelPlugin *plugin)
{
    SamplePyPlugin *sample_py;
    GObject *widget;

    sample_py = sample_py_new(plugin);

    // Getting pygobject from python object
    widget = pygobject_get(sample_py->py_object);

    // verifying pygobject
    if (!G_IS_OBJECT(widget))
    {
        printf("Entry is not a python gobject\n");
        return;
    }
    gtk_widget_show_all(GTK_WIDGET(widget));

    // Adding python widget in plugin
    gtk_container_add(GTK_CONTAINER(plugin), GTK_WIDGET(widget));

    xfce_panel_plugin_add_action_widget(plugin, GTK_WIDGET(widget));

    // show the about menue item and connect signal
    xfce_panel_plugin_menu_show_about(plugin);
    g_signal_connect(G_OBJECT(plugin), "about",
                     G_CALLBACK(sample_py_about), sample_py);

    g_signal_connect(G_OBJECT(plugin), "free-data",
                     G_CALLBACK(sample_py_free), sample_py);

    g_signal_connect(G_OBJECT(plugin), "orientation-changed",
                     G_CALLBACK(sample_py_orientation_changed), sample_py);
}