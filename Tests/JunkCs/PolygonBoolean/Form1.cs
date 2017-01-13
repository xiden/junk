using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Jk;

namespace PolygonBoolean {
	public partial class Form1 : Form {
		List<PolBoolF.Polygon>[] _Groups = new List<PolBoolF.Polygon>[] {
			new List<PolBoolF.Polygon>(),
			new List<PolBoolF.Polygon>(),
		};

		List<PolBoolF.Polygon> CurGroup {
			get {
				return _Groups[this.cmbCurGroup.SelectedIndex];
			}
		}

		PolBoolF.Polygon CurPolygon {
			get {
				var index = this.cmbCurPolygon.SelectedIndex;
				return 0 <= index ? this.CurGroup[index] : null;
			}
		}

		PolBoolF.Hole CurHole {
			get {
				var index = this.cmbCurHole.SelectedIndex;
				return 0 <= index ? this.CurPolygon.Holes[index] : null;
			}
		}


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

			this.cmbCurGroup.Items.Add("A");
			this.cmbCurGroup.Items.Add("B");
			this.cmbCurGroup.SelectedIndex = 0;

			cmbCurGroup_SelectedIndexChanged(this, EventArgs.Empty);

			this.cmbPolygonIndex.Items.Add(0);
			this.cmbPolygonIndex.Items.Add(1);
			this.cmbPolygonIndex.SelectedIndex = 0;

			UpdatePolygonCmb(true);

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

			//_Points[0].Add(ToPt(c + r1));
			//_Points[0].Add(ToPt(c + r2));
			//_Points[0].Add(ToPt(c + r3));
			//_Points[1].Add(ToPt(c + 2 * r1 / 3));
			//_Points[1].Add(ToPt(c + 2 * r2 / 3));
			//_Points[1].Add(ToPt(c + 2 * r3 / 3));

			//_Holes[0].Add(new List<Point>());
			//_Holes[0][0].Add(ToPt(c + r1 / 3));
			//_Holes[0][0].Add(ToPt(c + r3 / 3));
			//_Holes[0][0].Add(ToPt(c + r2 / 3));

			//_Holes[1].Add(new List<Point>());
			//_Holes[1][0].Add(ToPt(c + r1 / 6));
			//_Holes[1][0].Add(ToPt(c + r3 / 3));
			//_Holes[1][0].Add(ToPt(c + r2 / 6));

			//_Points[0].Add(new Point(50, 200));
			//_Points[0].Add(new Point(50, 300));
			//_Points[0].Add(new Point(70, 250));
			//_Points[1].Add(new Point(50, 200));
			//_Points[1].Add(new Point(50, 300));
			//_Points[1].Add(new Point(400, 250));
		}

		protected override void OnMouseDown(MouseEventArgs e) {
			base.OnMouseDown(e);

			var pt = new Vector2f((int)Math.Round(e.X / 10.0, MidpointRounding.AwayFromZero) * 10, (int)Math.Round(e.Y / 10.0, MidpointRounding.AwayFromZero) * 10);
			var group = this.CurGroup;

			if (e.Button == MouseButtons.Left) {
				if (group.Count == 0)
					btnAddPol_Click(this, EventArgs.Empty);

				var pol = this.CurPolygon;

				pol.Vertices.Add(new PolBoolF.Vertex(pt));
				this.Invalidate();
			}
			if (e.Button == MouseButtons.Right) {
				if (group.Count == 0)
					btnAddPol_Click(this, EventArgs.Empty);

				var pol = this.CurPolygon;

				if(pol.Holes == null || pol.Holes.Count == 0)
					btnAddHole_Click(this, EventArgs.Empty);

				var hole = this.CurHole;
				hole.Vertices.Add(new PolBoolF.Vertex(pt));
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

			var pb = new Jk.PolBoolF(1f);

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
				for (int i = 0; i < _Groups.Length; i++) {
					var group = _Groups[i];

					pb.AddPolygon(group);

					foreach (var polygon in group) {
						var vertices = polygon.Vertices;

						if (2 <= vertices.Count && vertices.Count < 3)
							g.DrawLines(penLine, (from v in vertices select ToPt(v.Position)).ToArray());
						else if (3 <= vertices.Count)
							g.DrawPolygon(penLine, (from v in vertices select ToPt(v.Position)).ToArray());
						foreach (var v in vertices) {
							g.DrawRectangle(penRect, v.Position.X - 2, v.Position.Y - 2, 4, 4);
						}

						if (polygon.Holes != null) {
							foreach (var hole in polygon.Holes) {
								vertices = hole.Vertices;
								if (2 <= vertices.Count && vertices.Count < 3)
									g.DrawLines(penHoleLine, (from v in vertices select ToPt(v.Position)).ToArray());
								else if (3 <= vertices.Count)
									g.DrawPolygon(penHoleLine, (from v in vertices select ToPt(v.Position)).ToArray());
								foreach (var v in vertices) {
									g.DrawRectangle(penHoleNode, v.Position.X - 2, v.Position.Y - 2, 4, 4);
								}
							}
						}
					}
				}

				try {
					// トポロジー化
					pb.CreateTopology(true);

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
					List<List<List<Jk.PolBoolF.EdgeAndSide>>> result = null;
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
					else if (radFilter.Checked) {
						var edgeFilter = new Func<PolBoolF.Edge, bool, bool>(
							(PolBoolF.Edge edge, bool right) => {
								List<int> rg, lg;
								int[] rp, lp;

								if (right) {
									rg = edge.RightGroups;
									lg = edge.LeftGroups;
									rp = edge.RightPolygons;
									lp = edge.LeftPolygons;
								} else {
									lg = edge.RightGroups;
									rg = edge.LeftGroups;
									lp = edge.RightPolygons;
									rp = edge.LeftPolygons;
								}
								var nr = rg.Count;
								var nl = lg.Count;

								// 進行方向右側にポリゴンが無ければ無視
								if (nr == 0)
									return true;

								// ポリゴン外との境界だったら無視はできない
								if (nl == 0)
									return false;



								// 左右のポリゴンの色を取得
								Color rc = Color.Empty, lc = Color.Empty;

								for (int i = 1; i != -1; i--) {
									if (rg.Contains(i)) {
										rc = (Color)_Groups[i][rp[i]].UserData;
										break;
									}
								}
								for (int i = 1; i != -1; i--) {
									if (lg.Contains(i)) {
										lc = (Color)_Groups[i][lp[i]].UserData;
										break;
									}
								}

								// 左右の色が違うなら無視できない
								if (rc != lc)
									return false;

								return true;
							}
						);
						result = pb.Filtering(edgeFilter);
					}
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
							(edge.Flags & Jk.PolBoolF.EdgeFlags.RightRemoved) != 0 ? penEdgeRightRemoved : penEdgeRight,
							ToPt(p1 - v), ToPt(p2 - v));
						g.DrawLine(
							(edge.Flags & Jk.PolBoolF.EdgeFlags.LeftRemoved) != 0 ? penEdgeLeftRemoved : penEdgeLeft,
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

						var size = g.MeasureString(edge.LeftGroups.Count.ToString(), this.Font, 1000, sf);

						g.DrawString(edge.LeftGroups.Count.ToString(), this.Font, brsFontEdge, pl.X - size.Width / 2, pl.Y - size.Height / 2, sf);


						size = g.MeasureString(edge.RightGroups.Count.ToString(), this.Font, 1000, sf);
						g.DrawString(edge.RightGroups.Count.ToString(), this.Font, brsFontEdge, pr.X - size.Width / 2, pr.Y - size.Height / 2, sf);
					}
					foreach (var node in pb.Nodes) {
						var p = node.Position;
						var pen = (node.Flags & Jk.PolBoolF.NodeFlags.InsideOutside) != 0 ? penNodeInsideOutside : penNode;
						g.DrawRectangle(pen, p.X - 2, p.Y - 2, 4, 4);
						g.DrawString(node.Edges.Count.ToString(), this.Font, brsFontNode, p.X, p.Y);
					}
					g.Transform = orgTf;

					// 結果のポリゴンを描画
					g.TranslateTransform(0, 500);
					for (int j = 0; j < result.Count; j++) {
						if (1 <= cmbPol.SelectedIndex && cmbPol.SelectedIndex != (j + 1))
							continue;
						var loops = result[j];
						for (int i = 0, n = loops.Count; i < n; i++) {
							if (1 <= cmbHole.SelectedIndex && cmbHole.SelectedIndex != (i + 1))
								continue;

							var loop = loops[i];
							Brush brs = i == 0 ? brsPolygon : brsHole;

							if (i == 0) {
								int group = -1, polygon = -1;

								foreach (var edge in loop) {
									List<int> rg;
									int[] rp;

									if (edge.TraceRight) {
										rg = edge.Edge.RightGroups;
										rp = edge.Edge.RightPolygons;
									} else {
										rg = edge.Edge.LeftGroups;
										rp = edge.Edge.LeftPolygons;
									}

									for (int ig = 1; ig != -1; ig--) {
										if (rg.Contains(ig)) {
											if (group < ig) {
												group = ig;
												polygon = rp[ig];
											}
										}
									}
								}

								brs = new SolidBrush((Color)_Groups[group][polygon].UserData);
							}

							g.FillPolygon(brs, (from edge in loop select ToPt(edge.From.Position)).ToArray());
						}
					}
					for (int j = 0; j < result.Count; j++) {
						if (1 <= cmbPol.SelectedIndex && cmbPol.SelectedIndex != (j + 1))
							continue;
						var loops = result[j];
						for (int i = 0, n = loops.Count; i < n; i++) {
							if (1 <= cmbHole.SelectedIndex && cmbHole.SelectedIndex != (i + 1))
								continue;
							var loop = loops[i];
							g.DrawPolygon(penPolygon, (from edge in loop select ToPt(edge.From.Position)).ToArray());
						}
					}

					//// 他の多角形との共有エッジを描画
					//var matcher = new Func<Jk.PolygonBooleanf.Edge, bool, bool>(
					//	(Jk.PolygonBooleanf.Edge edge, bool right) => {
					//		List<int> r, l;
					//		if (right) {
					//			r = edge.Right;
					//			l = edge.Left;
					//		} else {
					//			l = edge.Right;
					//			r = edge.Left;
					//		}
					//		if (!(r.Count != 1 || r[0] != this.cmbPolygonIndex.SelectedIndex || l.Count != 0))
					//			return false;
					//		return true;
					//	}
					//);

					//foreach (var polList in result) {
					//	foreach (var edges in polList) {
					//		var n = edges.Count;
					//		foreach (var indexRange in Jk.PolygonBooleanf.MatchSegments(edges, matcher)) {
					//			for (int i = 0; i < indexRange.Count; i++) {
					//				var edge = edges[(indexRange.Start + i) % n];
					//				var n1 = edge.Edge.From;
					//				var n2 = edge.Edge.To;
					//				g.DrawLine(penArc, n1.Position.X, n1.Position.Y, n2.Position.X, n2.Position.Y);
					//			}
					//		}
					//	}
					//}

					g.Transform = orgTf;

					this.label1.Text = sb.ToString();
				} catch (Exception ex) {
					this.lblResult.Text = ex.Message;
				}
			}
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
			this.CurGroup.Clear();
			this.Invalidate();
		}

		private void btnAddPol_Click(object sender, EventArgs e) {
			var pol = new PolBoolF.Polygon(new List<PolBoolF.Vertex>(), null, null);
			pol.UserData = Color.FromArgb(0, 127, 255);
			this.CurGroup.Add(pol);
			UpdatePolygonCmb(true);
			this.Invalidate();
		}

		private void btnDelPol_Click(object sender, EventArgs e) {
			var group = this.CurGroup;
			var index = this.cmbCurPolygon.SelectedIndex;
			if (0 <= index && index < group.Count)
				group.RemoveAt(index);
			UpdatePolygonCmb(false);
			this.Invalidate();
		}

		private void btnAddHole_Click(object sender, EventArgs e) {
			var group = this.CurGroup;
			if (group.Count == 0)
				group.Add(new PolBoolF.Polygon(new List<PolBoolF.Vertex>(), null, null));

			var polygon = group[group.Count - 1];

			var holes = polygon.Holes;
			if (holes == null)
				polygon.Holes = holes = new List<PolBoolF.Hole>();

			holes.Add(new PolBoolF.Hole(new List<PolBoolF.Vertex>(), null));

			UpdateHoleCmb(true);

			this.Invalidate();
		}

		private void btnDelHole_Click(object sender, EventArgs e) {
			var group = this.CurGroup;
			if (group.Count == 0)
				return;

			var polygon = group[group.Count - 1];

			var holes = polygon.Holes;
			if (holes == null)
				return;

			var index = this.cmbCurHole.SelectedIndex;
			if (0 <= index && index < holes.Count)
				polygon.Holes.RemoveAt(index);

			if (polygon.Holes.Count == 0)
				polygon.Holes = null;

			UpdateHoleCmb(false);

			this.Invalidate();
		}

		private void cmbPol_SelectedIndexChanged(object sender, EventArgs e) {
			this.Invalidate();
		}

		private void cmbHole_SelectedIndexChanged(object sender, EventArgs e) {
			this.Invalidate();
		}

		private void cmbCurGroup_SelectedIndexChanged(object sender, EventArgs e) {
			UpdatePolygonCmb(true);
		}

		private void cmbCurPolygon_SelectedIndexChanged(object sender, EventArgs e) {
			UpdateHoleCmb(true);
			var pol = this.CurPolygon;
			if (pol != null) {
				this.pictureBox1.BackColor = (Color)pol.UserData;
			}
		}

		void UpdatePolygonCmb(bool selectLast) {
			var group = this.CurGroup;
			var index = this.cmbCurPolygon.SelectedIndex;
			this.cmbCurPolygon.BeginUpdate();
			this.cmbCurPolygon.Items.Clear();
			for (int i = 1; i <= group.Count; i++) {
				this.cmbCurPolygon.Items.Add(i);
			}
			if (this.cmbCurPolygon.Items.Count != 0) {
				if (selectLast || index < 0 || this.cmbCurPolygon.Items.Count <= index)
					this.cmbCurPolygon.SelectedIndex = this.cmbCurPolygon.Items.Count - 1;
			}
			this.cmbCurPolygon.EndUpdate();

			if (this.cmbCurPolygon.SelectedIndex != index)
				UpdateHoleCmb(true);
		}

		void UpdateHoleCmb(bool selectLast) {
			var polygon = this.CurPolygon;
			if (polygon == null)
				return;

			var index = this.cmbCurHole.SelectedIndex;
			this.cmbCurHole.BeginUpdate();
			this.cmbCurHole.Items.Clear();
			if (polygon.Holes != null) {
				for (int i = 1; i <= polygon.Holes.Count; i++) {
					this.cmbCurHole.Items.Add(i);
				}
				if (this.cmbCurHole.Items.Count != 0) {
					if (selectLast || index < 0 || this.cmbCurHole.Items.Count <= index)
						this.cmbCurHole.SelectedIndex = this.cmbCurHole.Items.Count - 1;
				}
			}
			this.cmbCurHole.EndUpdate();
		}

		private void pictureBox1_Click(object sender, EventArgs e) {
			var pol = this.CurPolygon;
			if (pol == null)
				return;

			var cd = new ColorDialog();
			cd.Color = this.pictureBox1.BackColor;

			var r = cd.ShowDialog(this);
			if (r != DialogResult.OK)
				return;

			this.pictureBox1.BackColor = cd.Color;
			pol.UserData = cd.Color;

			this.Invalidate();
		}
	}
}
