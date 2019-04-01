%defines
%{


#include <cmath>  // for pow
#include <string>
#include "errmsg.h"
#include "get_opt.h"
#include "geom.h"

namespace {

typedef std::map<std::string, Number_type> param_map;
typedef std::map<std::string, size_t> name_index_map; 

param_map params;
name_index_map planar_names;

std::pair<Number_type,Number_type> boundaries;

std::vector<planar> planar_diels;

Number_type cur_ht = 0;

simple_poly shape;

simple_poly_cont polys;

size_t net_counter = 0;

} // end of anonymous namespace

void setlexer(std::string const& filename); 

position lexerPos();

bool lexerEOF();

int yylex(void); /* function prototype */

std::ostream& operator<< (std::ostream& out, const position& pos)
{
  if (!pos)
    return out;

  out << pos.filename() << ": ";
  out << pos.line() << "." << pos.column() << ": ";
  return out;
}

static void
yyerror (std::string const& msg, bool semantic = false)
{
  std::cout << lexerPos();
  if (semantic)
    std::cout << "semantic error: ";
  std::cout << msg << std::endl;
}

%}

/* %pure-parser */

/* %output="idia-parser.cpp" */

/* Exercise %union. */
%union {
  position pos;
  std::string *id;
  Number_type *dval;
  bool boo;
  struct {
    const char *sval;
    Number_type *dval;
  } conf;
  struct {
    Number_type *x,*y;
  } pt;
};


/* Bison Declarations */
%error-verbose
%token            MY_EOF 0    "end of input"
%token <id>       ID          "identifier"
%token <dval>     NUM         "number"
%token            PRINT       "print"
%token            PARAMS      "params"
%token            BOUNDARIES  "boundaries"
%token            DIRICHLET   "dirichlet"
%token            PLANARS     "planars"
%token            CONFORMALS  "conformals"
%token            CONDUCTORS  "conductors"
%token            EPS         "eps"
%token            UPTO        "upto"
%token            DIEL        "diel"
%token            NET         "net"
%token            SHAPE       "shape"
%token            JOIN        "--"

%type  <boo>  maybe_dirichlet
%type  <dval> exp
%type  <conf> name_or_num
%type  <pt>   point

%destructor { delete $$; } "identifier"
%destructor { delete $$; } "number"

%nonassoc '=' /* comparison	       */
%left '-' '+'
%left '*' '/'
%left NEG     /* negation--unary minus */
%right '^'    /* exponentiation        */

/* Grammar follows */
%%
idia:
  /* empty */
| idia section

section:
  params
| boundaries
| planars
| conformals
| conductors
;

boundaries:
  "boundaries" '(' exp ',' exp ')' maybe_dirichlet {
    boundaries = std::make_pair(*$3, *$5);
    dlog() << "-I- left boundary: " << boundaries.first
           << " right boundary: " << boundaries.second
           << " is dirichlet:" << std::boolalpha << " " << $7 << std::endl;
  }
;

maybe_dirichlet:
  /* empty */        { $$ = false; }
| "dirichlet"        { $$ = true;  }
;

params:
  "params" param_stmt_list
;

param_stmt_list:
  /* empty */
| param_stmt_list param_stmt
;

param_stmt:
  ID '=' exp ';'     { params[*$1] = *$3; delete $3; }
| "print" ID ';'     { std::cout << *$2 << ": " << CGAL::to_double(params[*$2]) << std::endl; }
| error      ';'
;

exp:
  NUM                { $$ = $1;                                          }
| ID                 { $$ = new Number_type(params[*$1]);                }
| exp '+' exp        { $$ = new Number_type(*$1 + *$3);                  }
| exp '-' exp        { $$ = new Number_type(*$1 - *$3);                  }
| exp '*' exp        { $$ = new Number_type(*$1 * *$3);                  }
| exp '/' exp        { $$ = new Number_type(*$1 / *$3);                  }
| '-' exp  %prec NEG { $$ = new Number_type(- *$2);                      }
| exp '^' exp        { $$ = new Number_type(pow (CGAL::to_double(*$1),
                                                 CGAL::to_double(*$3))); }
| '(' exp ')'        { $$ = $2;                                          }
| '(' error ')'      { $$ = new Number_type(0);                          }
;

planars:
  "planars"          { cur_ht = 0; planar_diels.clear(); /*planar_diels.push_back(planar("dummy"));*/ }
  planar_diels       { std::sort(planar_diels.begin(), planar_diels.end()); }
;

planar_diels:
  /* empty */
| planar_diels planar_diel
;

planar_diel:
  ID "eps" exp "upto" exp ';'
  {
    planar_diels.push_back(planar(*$1, *$3, cur_ht, *$5));
    size_t last_index = planar_diels.size() - 1;
    planar_names[*$1] = last_index;

    planar const& last_elem = planar_diels[last_index];
      dlog() << "-I- planar diel " << last_elem.name << " eps: " << last_elem.eps
             << " from: " << last_elem.from << " upto: " << last_elem.upto << std::endl;

    cur_ht = *$5;
  }
| error ';'
;

conformals:
  "conformals" conformal_diels
;

conformal_diels:
  /* empty */
| conformal_diels conformal_diel
;

conformal_diel:
  "diel" name_or_num shape ';' {
     dlog() << "-I- conformal: " << $2.sval << " eps: " << CGAL::to_double(*$2.dval) << std::endl;
     shape.name = $2.sval;
     shape.eps  = *$2.dval;
     shape.type = face_data::conformal;
     delete $2.dval;
     shape.fix_orientation();
     polys.push_back(shape);
  }
;

name_or_num:
  ID  {
        if (size_t diel_ind = planar_names[*$1]) {
          planar const& p = planar_diels[diel_ind];
          $$.sval = p.name.c_str();
          $$.dval = new Number_type(p.eps);
        } else {
          yyerror(std::string("planar dielectric '" + *$1 + "' is not defined in 'planars'"), true);
          $$.sval = "error";
          $$.dval = new Number_type(0);
        }
  }
| NUM {
        $$.sval = "<anonymous>";
        $$.dval = $1;
  }
| NUM '(' ID ')' {
        $$.sval = $3->c_str();
        $$.dval = $1;
  }
;

conductors:
  "conductors" nets
;

nets:
  /* empty */
| nets net
;

net:
  "net" ID shape ';'      { shape.name = *$2; shape.eps = 0.0;
                            shape.type = face_data::conductor;
                            shape.cond_num = ++net_counter;
                            shape.fix_orientation(); polys.push_back(shape); }
;

shape:
  "shape"                 { shape.reset(); }
              point       { shape.add(Point_2(*$3.x, *$3.y)); }
|  shape "--" point       { shape.add(Point_2(*$3.x, *$3.y)); }
;

point:
  '(' exp ',' exp ')' { $$.x = $2; $$.y = $4; }

%%

int
parse (std::vector<diel_segment<double> >& diel_segs, std::string const& fname)
{
  int status;

  setlexer(fname.c_str());

#if YYDEBUG
  yydebug = 1;
#endif

  status = yyparse ();

  Arrangement_2 arr;
  
  overlay_polygons (arr, boundaries, planar_diels, polys);

  gen_diel_segments (diel_segs, arr);

  dump_arrangement_faces (arr);

  return status;
}

