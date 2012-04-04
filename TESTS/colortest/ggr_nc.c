

struct colorinfo_t *ggrparse(const char *path) 
{
        struct palette_t *new;
        FILE *ggr;
        int i;

        new = malloc(sizeof(struct palette_t));

        ggr = fopen(path, "r");

        fscanf(ggr, "%a", new->name);
        fscanf(ggr, "%u", new->len);

        new->rgb = calloc(new->len, sizeof(new->rgb));

        for (i=0; i<new->len; i++) {
                fscanf(ggr, "%*f %*f %*f %f %f %f %*f %*f %*f %*f %*f %*d %*d",
                       new->rgb[i][_R_];
                       new->rgb[i][_G_];
                       new->rgb[i][_B_];
        }
        fclose(ggr);

        return (new);
}
