﻿using System;
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
		int _Current = 0;

		public Form1() {
			InitializeComponent();
		}

		protected override void OnLoad(EventArgs e) {
			base.OnLoad(e);

			this.cmbPolygonIndex.Items.Add(0);
			this.cmbPolygonIndex.Items.Add(1);
			this.cmbPolygonIndex.SelectedIndex = 0;
		}

		protected override void OnMouseDown(MouseEventArgs e) {
			base.OnMouseDown(e);
			if (e.Button == MouseButtons.Left) {
				_Points[_Current].Add(new Point((int)Math.Round(e.X / 10.0, MidpointRounding.AwayFromZero) * 10, (int)Math.Round(e.Y / 10.0, MidpointRounding.AwayFromZero) * 10));
				this.Invalidate();
			}
			if (e.Button == MouseButtons.Right) {
				_Points[_Current].Clear();
				this.Invalidate();
			}
		}

		protected override void OnMouseMove(MouseEventArgs e) {
			base.OnMouseMove(e);
		}

		protected override void OnPaint(PaintEventArgs e) {
			var g = e.Graphics;
			var cl = this.ClientRectangle;

			var pb = new Jk.PolygonBooleanf(1f);

			using (var penArc = new Pen(Color.FromArgb(255, 127, 0), 5))
			using (var brsPolygon = new SolidBrush(Color.Blue))
			using (var penPolygon = new Pen(Color.Green, 3))
			using (var penEdgeLeft = new Pen(Color.Blue, 5))
			using (var penEdgeRight = new Pen(Color.Green, 5))
			using (var penEdgeBoth = new Pen(Color.Black, 5))
			using (var penLine = new Pen(Color.Black))
			using (var penRect = new Pen(Color.Red))
			using (var penVolume = new Pen(Color.Blue)) {
				// 演算対象ポリゴンを登録
				for (int i = 0; i < _Points.Length; i++) {
					var points = _Points[i];
					var vertices = new List<Jk.PolygonBooleanf.Vertex>(from r in points select new Jk.PolygonBooleanf.Vertex(new Jk.Vector2f(r.X, r.Y)));
					var pol = new Jk.PolygonBooleanf.Polygon(vertices);

					pb.AddPolygon(pol);

					if (2 <= points.Count && points.Count < 3)
						g.DrawLines(penLine, points.ToArray());
					else if (3 <= points.Count)
						g.DrawPolygon(penLine, points.ToArray());
					foreach (var p in points) {
						g.DrawRectangle(penRect, p.X - 2, p.Y - 2, 4, 4);
					}
				}

				try {
					// トポロジー化
					var alleas = pb.CreateTopology();

					var sb = new StringBuilder();

					sb.AppendLine("ノード数: " + pb.Nodes.Count);
					sb.AppendLine("エッジ数: " + pb.Edges.Count);

					this.lblResult.Text = "";

					penEdgeRight.StartCap = System.Drawing.Drawing2D.LineCap.Round;
					penEdgeRight.EndCap = System.Drawing.Drawing2D.LineCap.ArrowAnchor;
					penEdgeLeft.StartCap = System.Drawing.Drawing2D.LineCap.Round;
					penEdgeLeft.EndCap = System.Drawing.Drawing2D.LineCap.ArrowAnchor;
					penEdgeBoth.StartCap = System.Drawing.Drawing2D.LineCap.Round;
					penEdgeBoth.EndCap = System.Drawing.Drawing2D.LineCap.ArrowAnchor;

					var orgTf = g.Transform;

					// エッジとノードを描画
					g.TranslateTransform(500, 0);
					foreach(var edge in pb.Edges) {
						var p1 = edge.From.Position;
						var p2 = edge.To.Position;
						Pen pen = null;
						if (edge.Left.Count != 0 && edge.Right.Count != 0)
							pen = penEdgeBoth;
						else if (edge.Right.Count != 0)
							pen = penEdgeRight;
						else if (edge.Left.Count != 0)
							pen = penEdgeLeft;

						g.DrawLine(pen, new PointF(p1.X, p1.Y), new PointF(p2.X, p2.Y));
					}
					foreach(var node in pb.Nodes) {
						var p = node.Position;
						g.DrawRectangle(penRect, p.X - 2, p.Y - 2, 4, 4);
					}
					g.Transform = orgTf;

					// 演算結果のポリゴンを描画
					List<List<Jk.PolygonBooleanf.EdgeAndSide>> result = null;
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

					g.TranslateTransform(0, 500);
					var polygons = new List<PointF[]>(
						from edges
						in result
						select (
							from node
							in Jk.PolygonBooleanf.NodesFromEdges(edges)
							select new PointF(node.Position.X, node.Position.Y)
						).ToArray()
					);
					foreach (var p in polygons) {
						g.FillPolygon(brsPolygon, p);
					}
					foreach (var p in polygons) {
						g.DrawPolygon(penPolygon, p);
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

					foreach(var edges in result) {
						var n = edges.Count;
						foreach(var indexRange in Jk.PolygonBooleanf.MatchArcs(edges, matcher)) {
							for(int i = 0; i < indexRange.Count; i++) {
								var edge = edges[(indexRange.Start + i) % n];
								var n1 = edge.Edge.From;
								var n2 = edge.Edge.To;
								g.DrawLine(penArc, n1.Position.X, n1.Position.Y, n2.Position.X, n2.Position.Y);
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
	}
}
