from fasthtml.common import *

css = Style('.square{display: flex; justify-content: center; align-items: center; background-color: antiquewhite} .square.black{background-color: gray} :root{--pico-grid-column-gap: 0px; --pico-grid-row-gap: 0px} .checker-piece{width: 80px; height: 80px; border-radius: 50%; background-color: #e74c3c;} .checker-piece.black{background-color: #34495e;}')

app = FastHTML(hdrs=(picolink, css))

SQUARE_SIZE = "100px"

init_black_pos = ['b77', 'b75', 'b73', 'b71', 'b66', 'b64', 'b62', 'b60', 'b57', 'b55', 'b53', 'b51']
init_red_pos = ['b00', 'b02', 'b04', 'b06', 'b11', 'b13', 'b15', 'b17', 'b20', 'b22', 'b24', 'b26']

def create_row(start_with_dark, row_id):
    colours = ['square black', 'square'] if start_with_dark else ['square', 'square black']

    row = []
    for col in range(8):
        cell_id = f'b{row_id}{col}'
        piece = None
        if cell_id in init_black_pos:
            piece = Div(cls='checker-piece black')
        elif cell_id in init_red_pos:
            piece = Div(cls='checker-piece')
        row.append(Div(piece, id=cell_id, cls=colours[col % 2], style=f'height: {SQUARE_SIZE};'))
    return row

rows = [Grid(*create_row(row % 2 == 0, row)) for row in range(8)]
board = Div(*rows, style='width: 800px')




def place_piece(position, colour):
    pass



@app.get("/")
def home():
    return Div(board)
