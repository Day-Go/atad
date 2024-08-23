from fasthtml.common import *

css = Style('.square{display: flex; justify-content: center; align-items: center; background-color: antiquewhite} .square.black{background-color: gray} :root{--pico-grid-column-gap: 0px; --pico-grid-row-gap: 0px} .checker-piece{width: 80px; height: 80px; border-radius: 50%; background-color: #e74c3c;} .checker-piece.black{background-color: #34495e;} .selected{box-shadow: 0 0 0 4px #fff;} .checker-piece.black.selected{box-shadow: 0 0 0 4px #000}')

app = FastHTML(hdrs=(picolink, css))

SQUARE_SIZE = "100px"

init_black_pos = ['b77', 'b75', 'b73', 'b71', 'b66', 'b64', 'b62', 'b60', 'b57', 'b55', 'b53', 'b51']
init_red_pos = ['b00', 'b02', 'b04', 'b06', 'b11', 'b13', 'b15', 'b17', 'b20', 'b22', 'b24', 'b26']

def create_row(start_with_dark, r):
    colours = ['square black', 'square'] if start_with_dark else ['square', 'square black']

    row = []
    for c in range(8):
        cell_id = f'b{r}{c}'
        piece_id = f'p{r}{c}'
        piece = None
        if cell_id in init_black_pos:
            piece = Div(id=piece_id, cls='checker-piece black', hx_on='click', hx_get=f'select/{piece_id}')
        elif cell_id in init_red_pos:
            piece = Div(id=piece_id, cls='checker-piece', hx_on='click', hx_get=f'select/{piece_id}')
        row.append(Div(piece, id=cell_id, cls=colours[c % 2], style=f'height: {SQUARE_SIZE};'))
    return row

rows = [Grid(*create_row(row % 2 == 0, row)) for row in range(8)]
board = Div(*rows, style='width: 800px')

@app.get("/")
def home():
    return Div(board)

@app.route('/select/{nm}')
def get(nm:str):
    js = f"""
    (function() {{
        const selectedPiece = document.getElementById('{nm}');
        if (selectedPiece) {{
            document.querySelectorAll('.checker-piece').forEach(piece => piece.classList.remove('selected'));
            selectedPiece.classList.add('selected');
        }} else {{
            console.error('Piece not found:', '{nm}');
        }}
    }})();
    """
    print(f"Selecting piece: {nm}")
    return Script(js)
