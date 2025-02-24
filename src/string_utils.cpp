#include "../headers/includes.hpp"
#include <unistd.h>

char	**ft_freestrs(char **strs, size_t words)
{
	size_t	i;

	i = 0;
	while (i < words)
	{
		free(strs[i]);
		i++;
	}
	free(strs);
	return (NULL);
}

static char	*ft_splitcpy(char *strs, char *s, size_t u, char c)
{
	size_t	i;

	i = 0;
	while (s[u] != c && s[u])
	{
		strs[i] = s[u];
		u++;
		i++;
	}
	strs[i] = '\0';
	return (strs);
}

static size_t	ft_splitmalloc(char **strs, char *s, char c, size_t u)
{
	size_t	i;

	i = 0;
	while (s[u] == c && s[u])
		u++;
	while (s[u] != c && s[u])
	{
		i++;
		u++;
	}
	*strs = (char *)malloc(sizeof(char) * (i + 1));
	if (!*strs)
		return (0);
	ft_splitcpy(*strs, s, u - i, c);
	return (u);
}

static size_t	ft_countwords(char const *s, char c)
{
	size_t	i;
	size_t	words;

	i = 0;
	words = 0;
	while (s[i])
	{
		while (s[i] == c && s[i])
			i++;
		if (s[i] != 0)
			words++;
		while (s[i] != c && s[i])
			i++;
	}
	return (words);
}

char	**ft_split(char const *s, char c)
{
	size_t	words;
	size_t	l;
	size_t	u;
	size_t	i;
	char	**strs;

	i = 0;
	u = 0;
	l = 0;
	if (!s)
		return (NULL);
	words = ft_countwords(s, c);
	strs = (char **)malloc(sizeof(char *) * (words + 1));
	if (!strs)
		return (NULL);
	while (i < words)
	{
		u = ft_splitmalloc(&strs[l], (char *)s, c, u);
		if (strs[l] == NULL)
			return (ft_freestrs(strs, i));
		l++;
		i++;
	}
	strs[l] = NULL;
	return (strs);
}