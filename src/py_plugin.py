#!/usr/bin/env python

import gi
gi.require_version('Gtk','3.0')
from gi.repository import Gtk

class PanelPlugin(Gtk.Box):
    """
    Xfce4 PanelPlugin,
    This class got called from the C interface of Plugin to embedd the
    resulting python object as Gtk Widget in Xfce4 panel.
    
    Gtk.Box is taken only as example, 
    Any gobject widget can be used as parent class
    """
    def __init__(self) -> None:
        """
        This method is called by sample_py_new() method
        """
        super().__init__()
        self.entry = Gtk.Entry()
        self.add(self.entry)

    def free(self):
        """
        Free method called by sample_py_free() when panel sends the "free"
        signal to plugin to clean up the allocations or post tasks
        like saving the configurations etc.
        """
        print("cleaning plugin from python side")


    def orientation_changed(self, orientation: int):
        """
        When the panel orientation changes then it emits a signal of
        orientation changed to all child plugins with current orientation

        Parameters:
            orientation (int): current orientation of plugin
                               0 = Gtk.Orientation.HORIZONTAL
                               1 = Gtk.Orientation.VERTICAL
        """
        print("Got Orientation:",orientation)