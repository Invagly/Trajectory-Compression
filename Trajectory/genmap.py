import numpy as np

class GenerateGoogleMap(object):
    """This class provides an API to generate google map for a list of trajectory files. """
    
    def __init__(self):
        pass
    
    def gen_googlemap(self, files, colors, zoom, clat, clng, wfn):
        """API for generate google map for a list of trajectory files.
        
        Keyword arguments:
        files  -- list of paths to the trajectory files
        colors -- list of colors for displaying the corresponding trajectories
        zoom   -- the initial zoom level
        clat   -- the initial latitude in the center
        clng   -- the initial longitude in the center
        wfn    -- path to the HTML file to be written
        """
        with open(wfn, 'w') as f:
            f.write(self._gen_header())
            f.write(self._js_initmap(zoom, clat, clng))
            for idx, fn in enumerate(files):
                f.write(self._gen_map_polyline(fn, idx, colors[idx]))
            f.write(self._gen_tail())
        
    def _gen_map_polyline(self, fn, idx, strokColor):
        jscripts = '\n\tvar cyclingpathcoords_' + str(idx) + ' = [\n'
        data = np.genfromtxt(fn, delimiter=',')
        latlng = lambda d: '\t\t{lat: ' + str(d[0]) + ', lng: ' + str(d[1]) + '},\n'
        for d in data:
            jscripts += latlng(d)
        jscripts += '\t];\n'
        
        jscripts += '\tvar cyclingpath_' + str(idx) + ' = new google.maps.Polyline({\n'
        jscripts += '\t\tpath: cyclingpathcoords_' + str(idx) + ',\n'
        jscripts += '\t\tgeodesic: true,\n'
        jscripts += '\t\tstrokeColor: \'' + strokColor + '\',\n'
        jscripts += '\t\tstrokeOpacity: 1.0,'
        jscripts += '\t\tstrokeWeight: 2\n'
        jscripts += '\t});\n'
        jscripts += '\tcyclingpath_' + str(idx) + '.setMap(map);\n'
        
        return jscripts
    
    def _js_initmap(self, zoom, lat, lng):
        jscripts = '\tvar map = new google.maps.Map(document.getElementById(\'map\'), {\n'
        jscripts += '\t\tzoom: ' + str(zoom) + ',\n'
        jscripts += '\t\tcenter: {lat:' + str(lat) + ', lng: ' + str(lng) + '},\n'
        jscripts += '\t\tmapTypeId: google.maps.MapTypeId.TERRAIN\n'
        jscripts += '\t});'
        return jscripts
    
    def _gen_header(self):
        header = ''
        with open('google_map_profile/head') as f:
            for L in f:
                header += L
        return header
    
    def _gen_tail(self):
        tail = ''
        with open('google_map_profile/tail') as f:
            for L in f:
                tail += L
        return tail
    
if __name__ == "__main__":
    g = GenerateGoogleMap()
    colors = ['#FF0005', '#0000FF']
    files = ['Data/Uncompressed/data.txt', 'Data/Compressed/compressList.txt']
    g.gen_googlemap(files, colors, 12, 39.9, 116.4, 'Data/trajectory.html')
    