import pickle
import wx
import getdist

scale = 200

class MapFrame(wx.Frame):
    def __init__(self, parent):
        super(MapFrame, self).__init__(parent, title = "MAP", size = (950, 800))
        self.InitUI()
    def InitUI(self):
        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Centre()
        self.Show(True)
    def OnPaint(self, e):
        global nodes, edges
        self.dc = wx.PaintDC(self)
        brush = wx.Brush("white")
        self.dc.SetBackground(brush)
        self.dc.Clear()

        pen = wx.Pen(wx.Colour(0,0,0))
        self.dc.SetPen(pen)
        for x in range(0, 950, 50):
            self.dc.DrawLine(x, 0, x, 800)
        for y in range(0, 800, 50):
            self.dc.DrawLine(0, y, 950, y)

        pen = wx.Pen(wx.Colour(0,0,255))
        self.dc.SetPen(pen)
        for edge in edges:
            node0 = nodes[edge[0]]
            node1 = nodes[edge[1]]
            self.dc.DrawLine(node0[0] / scale, node0[1] / scale, node1[0] / scale, node1[1] / scale)
            
        brush = wx.Brush("red")
        self.dc.SetBrush(brush)
        pen = wx.Pen("red")
        self.dc.SetPen(pen)
        for mark in marks:
            self.dc.DrawCircle(mark[0] / scale, mark[1] / scale, 2)

class TourFrame(wx.Frame):
    def __init__(self, parent):
        super(TourFrame, self).__init__(parent, title = "Tour", size = (900, 900))
        self.InitUI()
    def InitUI(self):
        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Centre()
        self.Show(True)
    def OnPaint(self, e):
        global nodes, edges, tours
        self.dc = wx.PaintDC(self)
        brush = wx.Brush("white")
        self.dc.SetBackground(brush)
        self.dc.Clear()

        #Draw grid
        pen = wx.Pen(wx.Colour(0,0,0))
        self.dc.SetPen(pen)
        for x in range(50, 800, 50):
            self.dc.DrawLine(x, 0, x, 800)
        for y in range(50, 800, 50):
            self.dc.DrawLine(0, y, 800, y)

        #Draw road
        pen = wx.Pen(wx.Colour(127, 127, 127))
        self.dc.SetPen(pen)
        for edge in edges:
            node0 = nodes[edge[0]]
            node1 = nodes[edge[1]]
            flag0, tmp0 = getpos(node0)
            flag1, tmp1 = getpos(node1)
            if flag0 or flag1:
                self.dc.DrawLine(tmp0[0], tmp0[1], tmp1[0], tmp1[1])

        colors = ["red", "blue", "yellow", "green", "pink"]

        #Draw lines
        for idx, ld in enumerate(tours[2]):
            brush = wx.Brush(colors[idx])
            self.dc.SetBrush(brush)
            pen = wx.Pen(colors[idx], 2)
            self.dc.SetPen(pen)
            for idy in range(1, len(ld)):
                flag, tmp0 = getpos(nodes[ld[idy - 1]])
                flag, tmp1 = getpos(nodes[ld[idy]])
                self.dc.DrawLine(tmp0[0], tmp0[1], tmp1[0], tmp1[1])

        #Draw nodes
        for idx, ld in enumerate(tours[1]):
            brush = wx.Brush(colors[idx])
            self.dc.SetBrush(brush)
            pen = wx.Pen("black")
            self.dc.SetPen(pen)
            for node in ld:
                flag, tmp = getpos(nodes[node])
                self.dc.DrawCircle(tmp[0], tmp[1], 5)

        #Draw st, ed
        brush = wx.Brush("black")
        self.dc.SetBrush(brush)
        pen = wx.Pen("black")
        self.dc.SetPen(pen)
        flag, tmp = getpos(tours[3][0])
        self.dc.DrawCircle(tmp[0], tmp[1], 5)
        flag, tmp = getpos(tours[3][1])
        self.dc.DrawCircle(tmp[0], tmp[1], 5)

class MainFrame(wx.Frame):
    def __init__(self, parent, *args, **kwargs):
        super(MainFrame, self).__init__(parent, *args, **kwargs)
        panel = wx.Panel(self)
        self.textst = wx.TextCtrl(panel, pos = (20, 20), size = (140, 40)) 
        self.texted = wx.TextCtrl(panel, pos = (20, 80), size = (140, 40)) 
        self.tourLabels = [wx.StaticText(panel, pos = (200, 20 + 120 * i)) for i in range(5)]
        button = wx.Button(panel, label = "search", pos = (20, 160), size = (140, 40))
        self.Bind(wx.EVT_BUTTON, self.search, button)
    def search(self, event):
        global nodes, marks, tours
        try:
            st = int(self.textst.GetValue())
            ed = int(self.texted.GetValue())
            marks = []
            marks.append(nodes[st])
            marks.append(nodes[ed])
        except:
            return
        res = getdist.search(nodes, cars, gridcars, st, ed)
        getTours(res, st, ed)
        self.refreshLabel(res, st, ed)
#        mapFrame = MapFrame(self)
#        tourFrame = TourFrame(self)
        #self.Close(True)
    def refreshLabel(self, res, st, ed):
        for idx, st in enumerate(res):
            nodeList = [cars[st[0]][0]]
            for t in st[1]:
                nodeList.append(t)
            tmptext = "->".join([str(t) for t in nodeList]) + "\nsbsb\n"
            self.tourLabels[idx].SetLabel(tmptext)

class App(wx.App):
    def OnInit(self):
        self.frame = MainFrame(None, title = "Taxi search", size = [800, 600])
        self.SetTopWindow(self.frame)
        self.frame.Show()
        return True

def getTours(res, start, end):
    global nodes, tours, marks
    tours[0] = []
    tours[1] = []
    tours[2] = []

    tmp = []
    for st in res:
        tmp[len(tmp) : len(tmp)] = [cars[st[0]][0]]
        tmp[len(tmp) : len(tmp)] = st[1]
    tmp = [nodes[i] for i in tmp]
    mnx = min([t[0] for t in tmp])
    mxx = max([t[0] for t in tmp])
    mny = min([t[1] for t in tmp])
    mxy = max([t[1] for t in tmp])
    side = max([mxx - mnx, mxy - mny])
    side = (int(side / 5000) + 2) * 5000
    left = max([0, int((mnx - (side - (mxx - mnx)) / 2) / 5000) * 5000])
    top = max([0, int((mny - (side - (mxy - mny)) / 2) / 5000) * 5000])
    tours[0] = [left, top, side]
    for idx, st in enumerate(res):
        tours[1].append([])
        tours[1][idx].append(cars[st[0]][0])
        for dt in st[1]:
            tours[1][idx].append(dt)
    tours[2] = tours[1]
    marks.append([left, top])
    marks.append([left, top + side])
    marks.append([left + side, top])
    marks.append([left + side, top + side])
    tours[3] = [nodes[start], nodes[end]]

def getpos(node):
    global tours
    x = tours[0][0]
    y = tours[0][1]
    a = tours[0][2]
    x0 = node[0]
    y0 = node[1]
    sc = a / 800
    if x0 > x and x0 < x + a and y0 > y and y0 < y + a:
        return True, [(x0 - x) / sc, (y0 - y) / sc]
    return False, [(x0 - x) / sc, (y0 - y) / sc]
if __name__ == "__main__":
    global marks, tours, edges, nodes, cars
    with open("../data/map.pkl", "rb") as f:
        nodes, edges = pickle.load(f)
    with open("../data/cars.pkl", "rb") as f:
        cars = pickle.load(f)
    with open("../data/gridcar.pkl", "rb") as f:
        gridcars = pickle.load(f)

    marks = []
    tours = [[], [], [], []]
    app = App(False)
    app.MainLoop()

