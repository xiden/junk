using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Triangulation {
	public partial class Form1 : Form {
		List<PointF> _Points = new List<PointF>();
		List<List<PointF>> _Holes = new List<List<PointF>>();

		public Form1() {
			InitializeComponent();
		}

		protected override void OnMouseDown(MouseEventArgs e) {
			base.OnMouseDown(e);

			var p = new PointF(e.X, e.Y);

			if (e.Button == MouseButtons.Left) {
				_Points.Add(p);
				this.Invalidate();
			} else if(e.Button == MouseButtons.Right) {
				if (_Holes.Count == 0)
					_Holes.Add(new List<PointF>());
				var hole = _Holes[_Holes.Count - 1];
				hole.Add(p);
				this.Invalidate();
			}
		}

		protected override void OnPaint(PaintEventArgs e) {
			base.OnPaint(e);

			var g = e.Graphics;

			using (var brsTri = new SolidBrush(Color.Green))
			using (var penPoint = new Pen(Color.Red))
			using (var penHole = new Pen(Color.DarkGray))
			using (var penLine = new Pen(Color.Black)) {
				if(3 <= _Points.Count) {
					g.DrawPolygon(penLine, _Points.ToArray());
				} else if (2 <= _Points.Count) {
					g.DrawLines(penLine, _Points.ToArray());
				}
				foreach(var p in _Points) {
					g.DrawRectangle(penPoint, p.X - 2, p.Y - 2, 5, 5);
				}

				foreach (var hole in _Holes) {
					if (3 <= hole.Count) {
						g.DrawPolygon(penHole, hole.ToArray());
					} else if (2 <= hole.Count) {
						g.DrawLines(penHole, hole.ToArray());
					}
					foreach (var p in hole) {
						g.DrawRectangle(penHole, p.X - 2, p.Y - 2, 5, 5);
					}
				}

				lblError.Text = "";
				try {
					if (3 <= _Points.Count) {
						var validAllHoles = true;
						foreach(var hole in _Holes) {
							if(hole.Count < 3) {
								validAllHoles = false;
								break;
							}
						}

						var points = _Points;
						if (validAllHoles) {
							points = Jk.TriangulationF.incorporateHolesIntoPolygon<PointF>(
								(p) => new Jk.Vector2f(p.X, p.Y),
								_Points,
								_Holes);
						}

						var result = new List<Jk.TriangulationF.TriIdx>();
						Jk.TriangulationF.triangulate<PointF>(
							(p) => new Jk.Vector2f(p.X, p.Y),
							points,
							result);

						var orgtf = g.Transform;
						g.TranslateTransform(0, 500);
						foreach (var tri in result) {
							var tripts = new PointF[] {
								points[tri.A],
								points[tri.B],
								points[tri.C],
							};
							g.FillPolygon(brsTri, tripts);
						}
						foreach (var tri in result) {
							var tripts = new PointF[] {
								points[tri.A],
								points[tri.B],
								points[tri.C],
							};
							g.DrawPolygon(penLine, tripts);
						}
						g.Transform = orgtf;
					}
				} catch(Exception ex) {
					lblError.Text = ex.Message;
				}
			}
		}

		private void btnClear_Click(object sender, EventArgs e) {
			_Points.Clear();
			_Holes.Clear();
			this.Invalidate();
		}

		private void btnAddHole_Click(object sender, EventArgs e) {
			_Holes.Add(new List<PointF>());
		}

		private void btnDelHole_Click(object sender, EventArgs e) {
			if (_Holes.Count == 0)
				return;
			_Holes.RemoveAt(_Holes.Count - 1);
			this.Invalidate();
		}
	}
}
