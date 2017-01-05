using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace PolygonBoolean {
	public partial class Form1 : Form {
		List<Point>[] _Points = new List<Point>[] {
			new List<Point>(),
			new List<Point>(),
		};
		List<List<Point>>[] _Holes = new List<List<Point>>[] {
			new List<List<Point>>(),
			new List<List<Point>>(),
		};
		int _Current = 0;

		public Form1() {
			InitializeComponent();
		}

		public static Point ToPt(Jk.Vector2i v) {
			return new Point(v.X, v.Y);
		}
		public static PointF ToPt(Jk.Vector2f v) {
			return new PointF(v.X, v.Y);
		}

		protected override void OnLoad(EventArgs e) {
			base.OnLoad(e);

			this.cmbPolygonIndex.Items.Add(0);
			this.cmbPolygonIndex.Items.Add(1);
			this.cmbPolygonIndex.SelectedIndex = 0;

			this.cmbPol.Items.Add("All pols");
			for (int i = 1; i <= 10; i++)
				this.cmbPol.Items.Add(i.ToString());

			this.cmbHole.Items.Add("All holes");
			for (int i = 1; i <= 10; i++)
				this.cmbHole.Items.Add(i.ToString());

			var c = new Jk.Vector2i(300, 300);
			var r1 = new Jk.Vector2i(0, -200);
			var r2 = new Jk.Vector2i(-200, 200);
			var r3 = new Jk.Vector2i(200, 200);


			_Points[0].Add(ToPt(c + r1));
			_Points[0].Add(ToPt(c + r2));
			_Points[0].Add(ToPt(c + r3));
			_Points[1].Add(ToPt(c + 2 * r1 / 3));
			_Points[1].Add(ToPt(c + 2 * r2 / 3));
			_Points[1].Add(ToPt(c + 2 * r3 / 3));

			_Holes[0].Add(new List<Point>());
			_Holes[0][0].Add(ToPt(c + r1 / 3));
			_Holes[0][0].Add(ToPt(c + r3 / 3));
			_Holes[0][0].Add(ToPt(c + r2 / 3));

			_Holes[1].Add(new List<Point>());
			_Holes[1][0].Add(ToPt(c + r1 / 6));
			_Holes[1][0].Add(ToPt(c + r3 / 3));
			_Holes[1][0].Add(ToPt(c + r2 / 6));

			//_Points[0].Add(new Point(50, 200));
			//_Points[0].Add(new Point(50, 300));
			//_Points[0].Add(new Point(70, 250));
			//_Points[1].Add(new Point(50, 200));
			//_Points[1].Add(new Point(50, 300));
			//_Points[1].Add(new Point(400, 250));
		}

		protected override void OnMouseDown(MouseEventArgs e) {
			base.OnMouseDown(e);

			var pt = new Point((int)Math.Round(e.X / 10.0, MidpointRounding.AwayFromZero) * 10, (int)Math.Round(e.Y / 10.0, MidpointRounding.AwayFromZero) * 10);

			if (e.Button == MouseButtons.Left) {
				_Points[_Current].Add(pt);
				this.Invalidate();
			}
			if (e.Button == MouseButtons.Right) {
				var holes = _Holes[_Current];
				if (holes.Count == 0)
					holes.Add(new List<Point>());
				var hole = holes[holes.Count - 1];
				hole.Add(pt);
				this.Invalidate();
			}
		}

		protected override void OnMouseMove(MouseEventArgs e) {
			base.OnMouseMove(e);
			lblPos.Text = e.X + ", " + e.Y;
		}

		protected override void OnPaint(PaintEventArgs e) {
			var g = e.Graphics;
			var cl = this.ClientRectangle;

			var pb = new Jk.PolygonBooleanf(1f);

			using (var brsFontNode = new SolidBrush(Color.FromArgb(64, 64, 255)))
			using (var brsFontEdge = new SolidBrush(Color.FromArgb(255, 64, 64)))
			using (var penArc = new Pen(Color.FromArgb(255, 127, 0), 5))
			using (var brsPolygon = new SolidBrush(Color.Blue))
			using (var brsHole = new SolidBrush(Color.White))
			using (var penPolygon = new Pen(Color.Green, 1))
			using (var penEdgeLeft = new Pen(Color.FromArgb(0, 255, 0), 3))
			using (var penEdgeRight = new Pen(Color.FromArgb(255, 0, 0), 3))
			using (var penEdgeLeftRemoved = new Pen(Color.FromArgb(0, 127, 0), 3))
			using (var penEdgeRightRemoved = new Pen(Color.FromArgb(127, 0, 0), 3))
			using (var penEdgeBoth = new Pen(Color.Black, 5))
			using (var penLine = new Pen(Color.Black))
			using (var penHoleLine = new Pen(Color.DarkGray))
			using (var penRect = new Pen(Color.Red))
			using (var penHoleNode = new Pen(Color.DarkGray))
			using (var penNode = new Pen(Color.Red))
			using (var penNodeInsideOutside = new Pen(Color.Orange))
			using (var penVolume = new Pen(Color.Blue)) {
				// 演算対象ポリゴンを登録
				for (int i = 0; i < _Points.Length; i++) {
					var points = _Points[i];
					var holes = _Holes[i];
					var vertexList = new List<Jk.PolygonBooleanf.Vertex>(
						from p
						in points
						select new Jk.PolygonBooleanf.Vertex(new Jk.Vector2f(p.X, p.Y)));
					var holeList = new List<Jk.PolygonBooleanf.Hole>(
						from hole
						in holes
						select
							new Jk.PolygonBooleanf.Hole(
								new List<Jk.PolygonBooleanf.Vertex>(
									from p
									in hole
									select new Jk.PolygonBooleanf.Vertex(new Jk.Vector2f(p.X, p.Y))),
								null));
					var pol = new Jk.PolygonBooleanf.Polygon(vertexList, null, holeList);

					pb.AddPolygon(pol);

					if (2 <= points.Count && points.Count < 3)
						g.DrawLines(penLine, points.ToArray());
					else if (3 <= points.Count)
						g.DrawPolygon(penLine, points.ToArray());
					foreach (var p in points) {
						g.DrawRectangle(penRect, p.X - 2, p.Y - 2, 4, 4);
					}

					foreach (var hole in holes) {
						if (2 <= hole.Count && hole.Count < 3)
							g.DrawLines(penHoleLine, hole.ToArray());
						else if (3 <= hole.Count)
							g.DrawPolygon(penHoleLine, hole.ToArray());
						foreach (var p in hole) {
							g.DrawRectangle(penHoleNode, p.X - 2, p.Y - 2, 4, 4);
						}
					}
				}

				try {
					// トポロジー化
					pb.CreateTopology();

					var sb = new StringBuilder();

					sb.AppendLine("ノード数: " + pb.Nodes.Count);
					sb.AppendLine("エッジ数: " + pb.Edges.Count);

					this.lblResult.Text = "";

					penEdgeRight.StartCap = System.Drawing.Drawing2D.LineCap.Round;
					penEdgeRight.EndCap = System.Drawing.Drawing2D.LineCap.ArrowAnchor;
					penEdgeLeft.StartCap = System.Drawing.Drawing2D.LineCap.Round;
					penEdgeLeft.EndCap = System.Drawing.Drawing2D.LineCap.ArrowAnchor;
					penEdgeRightRemoved.StartCap = System.Drawing.Drawing2D.LineCap.Round;
					penEdgeRightRemoved.EndCap = System.Drawing.Drawing2D.LineCap.ArrowAnchor;
					penEdgeLeftRemoved.StartCap = System.Drawing.Drawing2D.LineCap.Round;
					penEdgeLeftRemoved.EndCap = System.Drawing.Drawing2D.LineCap.ArrowAnchor;
					penEdgeBoth.StartCap = System.Drawing.Drawing2D.LineCap.Round;
					penEdgeBoth.EndCap = System.Drawing.Drawing2D.LineCap.ArrowAnchor;
					penPolygon.StartCap = System.Drawing.Drawing2D.LineCap.Round;
					penPolygon.EndCap = System.Drawing.Drawing2D.LineCap.Round;
					penPolygon.DashCap = System.Drawing.Drawing2D.DashCap.Round;

					var orgTf = g.Transform;

					// ポリゴン同士の演算を行う
					List<List<List<Jk.PolygonBooleanf.EdgeAndSide>>> result = null;
					if (radOr.Checked)
						result = pb.Or();
					else if (radXor.Checked)
						result = pb.Xor();
					else if (radAnd.Checked)
						result = pb.And();
					else if (radSub.Checked)
						result = pb.Sub(this.cmbPolygonIndex.SelectedIndex);
					else if (radExtract.Checked)
						result = pb.Extract(this.cmbPolygonIndex.SelectedIndex);
					sb.AppendLine("演算結果ポリゴン数: " + result.Count);
					foreach (var r in result) {
						sb.AppendLine((r.Count - 1).ToString());
					}

					// エッジとノードを描画
					g.TranslateTransform(500, 0);
					foreach (var edge in pb.Edges) {
						var p1 = edge.From.Position;
						var p2 = edge.To.Position;
						var v = (edge.To.Position - edge.From.Position).Normalize().VerticalCw();

						v *= 2;
						g.DrawLine(
							(edge.Flags & Jk.PolygonBooleanf.EdgeFlags.RightRemoved) != 0 ? penEdgeRightRemoved : penEdgeRight,
							ToPt(p1 - v), ToPt(p2 - v));
						g.DrawLine(
							(edge.Flags & Jk.PolygonBooleanf.EdgeFlags.LeftRemoved) != 0 ? penEdgeLeftRemoved : penEdgeLeft,
							ToPt(p1 + v), ToPt(p2 + v));
					}
					foreach (var edge in pb.Edges) {
						var c = (edge.From.Position + edge.To.Position) / 2;
						var v = (edge.To.Position - edge.From.Position).Normalize().VerticalCw();
						v *= 10;
						var pl = c + v;
						var pr = c - v;

						var sf = new StringFormat(StringFormat.GenericTypographic);
						sf.FormatFlags = sf.FormatFlags & ~StringFormatFlags.LineLimit | StringFormatFlags.NoWrap; // StringFormatFlagsLineLimit があると計算誤差の関係で文字が非表示になるので消す

						var size = g.MeasureString(edge.Left.Count.ToString(), this.Font, 1000, sf);

						g.DrawString(edge.Left.Count.ToString(), this.Font, brsFontEdge, pl.X - size.Width / 2, pl.Y - size.Height / 2, sf);


						size = g.MeasureString(edge.Right.Count.ToString(), this.Font, 1000, sf);
						g.DrawString(edge.Right.Count.ToString(), this.Font, brsFontEdge, pr.X - size.Width / 2, pr.Y - size.Height / 2, sf);
					}
					foreach (var node in pb.Nodes) {
						var p = node.Position;
						var pen = (node.Flags & Jk.PolygonBooleanf.NodeFlags.InsideOutside) != 0 ? penNodeInsideOutside : penNode;
						g.DrawRectangle(pen, p.X - 2, p.Y - 2, 4, 4);
						g.DrawString(node.Edges.Count.ToString(), this.Font, brsFontNode, p.X, p.Y);
					}
					g.Transform = orgTf;

					// 結果のポリゴンを描画
					g.TranslateTransform(0, 500);
					var polygonLists = (
						from pols
						in result
						select (
							from edges
							in pols
							select (
								from node
								in Jk.PolygonBooleanf.NodesFromEdges(edges)
								select new PointF(node.Position.X, node.Position.Y)
							).ToArray()
						).ToArray()
					).ToArray();
					for (int j = 0; j < polygonLists.Length; j++) {
						if (1 <= cmbPol.SelectedIndex && cmbPol.SelectedIndex != (j + 1))
							continue;
						var pl = polygonLists[j];
						for (int i = 0, n = pl.Length; i < n; i++) {
							if (1 <= cmbHole.SelectedIndex && cmbHole.SelectedIndex != (i + 1))
								continue;
							g.FillPolygon(i == 0 ? brsPolygon : brsHole, pl[i]);
						}
					}
					for (int j = 0; j < polygonLists.Length; j++) {
						if (1 <= cmbPol.SelectedIndex && cmbPol.SelectedIndex != (j + 1))
							continue;
						var pl = polygonLists[j];
						for (int i = 0, n = pl.Length; i < n; i++) {
							if (1 <= cmbHole.SelectedIndex && cmbHole.SelectedIndex != (i + 1))
								continue;
							g.DrawPolygon(penPolygon, pl[i]);
						}
					}

					// 他の多角形との共有エッジを描画
					var matcher = new Func<Jk.PolygonBooleanf.Edge, bool, bool>(
						(Jk.PolygonBooleanf.Edge edge, bool right) => {
							List<int> r, l;
							if (right) {
								r = edge.Right;
								l = edge.Left;
							} else {
								l = edge.Right;
								r = edge.Left;
							}
							if (!(r.Count != 1 || r[0] != this.cmbPolygonIndex.SelectedIndex || l.Count != 0))
								return false;
							return true;
						}
					);

					foreach (var polList in result) {
						foreach (var edges in polList) {
							var n = edges.Count;
							foreach (var indexRange in Jk.PolygonBooleanf.MatchSegments(edges, matcher)) {
								for (int i = 0; i < indexRange.Count; i++) {
									var edge = edges[(indexRange.Start + i) % n];
									var n1 = edge.Edge.From;
									var n2 = edge.Edge.To;
									g.DrawLine(penArc, n1.Position.X, n1.Position.Y, n2.Position.X, n2.Position.Y);
								}
							}
						}
					}

					g.Transform = orgTf;

					this.label1.Text = sb.ToString();
				} catch (Exception ex) {
					this.lblResult.Text = ex.Message;
				}
			}
		}

		private void radioButton1_CheckedChanged(object sender, EventArgs e) {
			_Current = 0;
		}

		private void radioButton2_CheckedChanged(object sender, EventArgs e) {
			_Current = 1;
		}

		private void radOr_CheckedChanged(object sender, EventArgs e) {
			this.Invalidate();
		}

		private void radSub_CheckedChanged(object sender, EventArgs e) {
			this.Invalidate();
		}

		private void radAnd_CheckedChanged(object sender, EventArgs e) {
			this.Invalidate();
		}

		private void radXor_CheckedChanged(object sender, EventArgs e) {
			this.Invalidate();
		}

		private void cmbPolygonIndex_SelectedIndexChanged(object sender, EventArgs e) {
			this.Invalidate();
		}

		private void radExtract_CheckedChanged(object sender, EventArgs e) {
			this.Invalidate();
		}

		private void btnClear_Click(object sender, EventArgs e) {
			_Points[_Current].Clear();
			_Holes[_Current].Clear();
			this.Invalidate();
		}

		private void btnAddHole_Click(object sender, EventArgs e) {
			_Holes[_Current].Add(new List<Point>());
			this.Invalidate();
		}

		private void btnDelHole_Click(object sender, EventArgs e) {
			var holes = _Holes[_Current];
			if (holes.Count == 0)
				return;
			holes.RemoveAt(holes.Count - 1);
			this.Invalidate();
		}

		private void cmbPol_SelectedIndexChanged(object sender, EventArgs e) {
			this.Invalidate();
		}

		private void cmbHole_SelectedIndexChanged(object sender, EventArgs e) {
			this.Invalidate();
		}
	}
}
