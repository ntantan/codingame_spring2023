char *mirrorInput(char *inputTxt)
{
        //YOUR CODE HERE//
        int size = 0;
        while (inputTxt[size])
                size++;

        int i = 0;
        char res[size];
        while (i < size)
        {
			res[i] = inputTxt[size - i - 1];
			i++;
        }
		res[size] = '\0';
        printf("%s", res);
        // return (res);
}

int main()
{
	mirrorInput("TEST");
	return (0);
}