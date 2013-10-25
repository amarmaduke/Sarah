
## Internal Language

The internal language supports the following values:

    n ::= Z             # integers
    
    v ::= [a-z][A-Z]*   # variable names
    
    e ::= n             # Integer literals
        | v             # Variable names
        | add(e1, e2)   # e1 + e2
        | neg(e)        # -e
        | mul(n, e)     # Repeated addition of e n times
        | eq(e1, e2)    # e1 == e2
        | ne(e1, e2)    # e1 != e2
        | lt(e1, e2)    # e1 < e2
        | gt(e1, e2)    # e1 > e2
        | le(e1, e2)    # e1 <= e2
        | ge(e1, e2)    # e1 >= e2
        | and(e1, e2)   # e1 and e2
        | or(e1, e2)    # e1 or e2
        | not(e)        # not e2
        | exists(v, e)  # exists v.e
        | forall(v, e)  # forall v.e

The following types are required by the language

    t ::= int | bool

### Type Inference rules ###


    ------------ T-int
    G |- n : int


    v in G
    ------------ T-var
    G |- v : int


    G |- e1 : int
    G |- e1 : int
    ---------------------- T-add
    G |- add(e1, e2) : int


    G |- e : int
    ---------------------- T-neg
    G |- add(e1, e2) : int


    G |- e : int
    ---------------------- T-mul
    G |- mul(n, e) : int

    ...


    G |- e : bool
    ------------------------ T-forall
    G |- forall(n, e) : bool

