import pickle
import wx

scale = 200

class MapFrame(wx.Frame):
    def __init__(self, parent, title):
        super(MapFrame, self).__init__(parent, title = title, size = (950, 800))
        self.InitUI()
    def InitUI(self):
        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Centre()
        self.Show(True)
    def OnPaint(self, e):
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
        for mark in marks:
            self.dc.DrawCircle(mark[0] / scale, mark[1] / scale, 2)

class MainFrame(wx.Frame):
    def __init__(self, parent, *args, **kwargs):
        super(MainFrame, self).__init__(parent, *args, **kwargs)
        panel = wx.Panel(self)
        self.mapFrame = MapFrame(panel, "Map")

        text = wx.TextCtrl(panel, pos = (20, 20), size = (140, 40)) 
        button = wx.Button(panel, label = "close", pos = (20, 80), size = (140, 40))
        self.Bind(wx.EVT_BUTTON, self.OnCloseMe, button)
    def OnCloseMe(self, event):
        marks.append([10000, 10000])
        self.mapFrame.Refresh(False)
        #self.Close(True)

class App(wx.App):
    def OnInit(self):
        self.frame = MainFrame(None, title = "Taxi search", size = [200, 500])
        self.SetTopWindow(self.frame)
        self.frame.Show()
        return True

if __name__ == "__main__":
    with open("../data/map.pkl", "rb") as f:
        (nodes, edges) = pickle.load(f)
    marks = []
    app = App(False)
    app.MainLoop()

