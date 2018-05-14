import pickle
import wx

scale = 200

class MapUI(wx.Frame):
	def __init__(self, parent, title):
		super(MapUI, self).__init__(parent, title = title, size = (950, 800))
		self.InitUI()
	def InitUI(self):
		self.Bind(wx.EVT_PAINT, self.OnPaint)
		self.Centre()
		self.Show(True)
	def OnPaint(self, e):
		dc = wx.PaintDC(self)
		brush = wx.Brush("white")
		dc.SetBackground(brush)
		dc.Clear()

		pen = wx.Pen(wx.Colour(0,0,0))
		dc.SetPen(pen)
		brush = wx.Brush("red")
		for x in range(0, 950, 50):
			dc.DrawLine(x, 0, x, 800)
		for y in range(0, 800, 50):
			dc.DrawLine(0, y, 950, y)

		pen = wx.Pen(wx.Colour(0,0,255))
		dc.SetPen(pen)
		for edge in edges:
			node0 = nodes[edge[0]]
			node1 = nodes[edge[1]]
			dc.DrawLine(node0[0] / scale, node0[1] / scale, node1[0] / scale, node1[1] / scale)


#		dc.DrawBitmap(wx.Bitmap("python.jpg"),10,10,True) 
#		color = wx.Colour(255,0,0)
#		b = wx.Brush(color) 
#		dc.SetBrush(b) 
#		dc.DrawCircle(300,125,50) 
#		dc.SetBrush(wx.Brush(wx.Colour(255,255,255))) 
#		dc.DrawCircle(300,125,30) 
#		font = wx.Font(18, wx.ROMAN, wx.ITALIC, wx.NORMAL) 
#		dc.SetFont(font) 
#		dc.DrawText("Hello wxPython",200,10) 
#		pen = wx.Pen(wx.Colour(0,0,255)) 
#		dc.SetPen(pen) 
#		dc.DrawLine(200,50,350,50) 
#		dc.SetBrush(wx.Brush(wx.Colour(0,255,0), wx.CROSS_HATCH)) 
#		dc.DrawRectangle(380, 15, 90, 60)

if __name__ == "__main__":
	with open("../data/map.pkl", "rb") as f:
		(nodes, edges) = pickle.load(f)
	app = wx.App()
	MapUI(None, 'TAXI Searcher')
	app.MainLoop()

